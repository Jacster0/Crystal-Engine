#include "ResourceStateTracker.h"
#include "../D3D12CommandContext.h"
#include "../D3D12Texture.h"

#include "Core/Logging/Logger.h"

using namespace Crystal;

MutexLock ResourceStateTracker::m_lock;
std::unordered_map<ID3D12Resource*, ResourceStateTracker::ResourceState> ResourceStateTracker::m_globalResourceState;

ResourceStateTracker::ResourceStateTracker() noexcept = default;

void ResourceStateTracker::ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier) noexcept {
	constexpr int allSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION) {
		const auto& transitionBarrier = barrier.Transition;

		//First check if there is already a known final state for the given resource
		//If there is, the resource has been used on the commandList before and
		//already has a known state within the commandList execution.
		const auto iter = m_finalResourceState.find(transitionBarrier.pResource);

		if (iter != m_finalResourceState.end()) {
			auto& resourceState = iter->second;

			//if the known final state of the resource is different
			if (transitionBarrier.Subresource == allSubResources && !resourceState.SubresourceState.empty()) {
				for (const auto& subResourceState : resourceState.SubresourceState) {
					if (transitionBarrier.StateAfter != subResourceState.second) {
						auto newBarrier                   = barrier;
						newBarrier.Transition.Subresource = subResourceState.first;
						newBarrier.Transition.StateBefore = subResourceState.second;
							
						m_resourceBarriers.push_back(newBarrier);
					}
				}
			}
			else {
				const auto finalState = resourceState.GetSubresourceState(transitionBarrier.Subresource);

				if (transitionBarrier.StateAfter != finalState) {
					//Push a new transition barrier with the correct before state.
					auto newBarrier                   = barrier;
					newBarrier.Transition.StateBefore = finalState;

					m_resourceBarriers.push_back(newBarrier);
				}
			}
		}
		else {
			// In this case, the resource is being used on the command list for the first time.
		    // Add a pending barrier. The pending barriers will be resolved
		    // before the command list is executed on the command queue.
			m_pendingResourceBarriers.push_back(barrier);
		}

		m_finalResourceState[transitionBarrier.pResource].SetSubResource(transitionBarrier.Subresource, transitionBarrier.StateAfter);
	}
	else {
		m_resourceBarriers.push_back(barrier);
	}
}

void ResourceStateTracker::TransitionResource(const Texture& texture, D3D12_RESOURCE_STATES stateAfter, uint32_t subResource) noexcept {
	TransitionResource(texture.GetUnderlyingResource().Get(), stateAfter, subResource);
}

void ResourceStateTracker::TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter, uint32_t subResource) noexcept {
	if (resource) {
		ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(
			resource,
			D3D12_RESOURCE_STATE_COMMON,
			stateAfter,
			subResource));
	}
}

void ResourceStateTracker::UAVBarrier(const Texture* const resource) noexcept {
	ResourceBarrier(CD3DX12_RESOURCE_BARRIER::UAV(resource->GetUnderlyingResource().Get()));
}

void ResourceStateTracker::AliasBarrier(const Texture* const resourceBefore, const Texture* const resourceAfter) noexcept {
	ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Aliasing(
		resourceBefore->GetUnderlyingResource().Get(), 
		resourceAfter->GetUnderlyingResource().Get()));
}

uint32_t ResourceStateTracker::FlushPendingResourceBarriers(const CommandContext* context) {
	if (m_lock.UnLocked()) {
		crylog_warning("The ResourceStateTracker is unlocked and is therefore not threadsafe!");
	}

	// Resolve the pending resource barriers by checking the global state of the 
    // (sub)resources. Add barriers if the pending state and the global state do
    //  not match.
	std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers;
	// Reserve enough space (worst-case, all pending barriers).
	resourceBarriers.reserve(m_pendingResourceBarriers.size());

	for (auto pendingBarrier : m_pendingResourceBarriers) {
		// Only transition barriers should be pending.
		if (pendingBarrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION) {
			const auto pendingTransition = pendingBarrier.Transition;

			const auto& iter = m_globalResourceState.find(pendingTransition.pResource);

			if (iter != m_globalResourceState.end()) {
				// If all subresources are being transitioned, and there are multiple
				// subresources of the resource that are in a different state
				const auto& resourceState = iter->second;

				if (pendingTransition.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
					!resourceState.SubresourceState.empty()) {
					// Transition all subresources
					for (const auto& subresourceState : resourceState.SubresourceState) {
						if (pendingTransition.StateAfter != subresourceState.second) {
							auto newBarrier                   = pendingBarrier;
							newBarrier.Transition.Subresource = subresourceState.first;
							newBarrier.Transition.StateBefore = subresourceState.second;

							resourceBarriers.push_back(newBarrier);
						}
					}
				}
				else {
					// No (sub)resources need to be transitioned. Just add a single transition barrier (if needed).
					const auto globalState = iter->second.GetSubresourceState(pendingTransition.Subresource);

					if (pendingTransition.StateAfter != globalState) {
						pendingBarrier.Transition.StateBefore = globalState;
						resourceBarriers.push_back(pendingBarrier);
					}
				}
			}
		}
	}

	const auto numBarriers = static_cast<uint32_t>(resourceBarriers.size());

	if (numBarriers > 0) {
		const auto d3d12CommandList = context->GetNativeCommandList();
		d3d12CommandList->ResourceBarrier(numBarriers, resourceBarriers.data());
	}

	m_pendingResourceBarriers.clear();

	return numBarriers;
}

void ResourceStateTracker::FlushResourceBarriers(const CommandContext* context) {
	if (!m_resourceBarriers.empty()) {
		const auto d3d12CommandList = context->GetNativeCommandList();
		d3d12CommandList->ResourceBarrier(static_cast<uint32_t>(m_resourceBarriers.size()), m_resourceBarriers.data());

		m_resourceBarriers.clear();
	}
}

void ResourceStateTracker::CommitFinalResourceStates() {
	if (m_lock.UnLocked()) {
		crylog_warning("The ResourceStateTracker is unlocked and is therefore not threadsafe!");
	}

	for (const auto& resourceState : m_finalResourceState) {
		m_globalResourceState[resourceState.first] = resourceState.second;
	}

	m_finalResourceState.clear();
}

void Crystal::ResourceStateTracker::Reset() noexcept {
	m_pendingResourceBarriers.clear();
	m_resourceBarriers.clear();
	m_finalResourceState.clear();
}

void ResourceStateTracker::Lock() noexcept {
	if (m_lock.UnLocked()) [[likely]] {
		m_lock.Lock();
	}
}

void ResourceStateTracker::Unlock() noexcept {
	if (m_lock.Locked()) [[likely]] {
		m_lock.Unlock();
	}
}

void ResourceStateTracker::AddGlobalResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state) noexcept {
	if (resource) {
		std::scoped_lock lock(m_lock.Mutex);
		m_globalResourceState[resource].SetSubResource(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, state);
	}
}
