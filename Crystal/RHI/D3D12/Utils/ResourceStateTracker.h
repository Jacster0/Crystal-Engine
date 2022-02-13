#pragma once

#include "d3dx12.h"

#include <wrl/client.h>
#include <mutex>
#include <map>
#include <unordered_map>
#include <vector>

namespace Crystal {
	

	struct MutexLock {
		void Lock() noexcept {
			Mutex.lock();
			m_lock = Lock_t::Locked;
		}
		void Unlock() noexcept {
			Mutex.unlock();
			m_lock = Lock_t::Unlocked;
		}
		[[nodiscard]] constexpr bool Locked() const noexcept { return m_lock == Lock_t::Locked; }
		[[nodiscard]] constexpr bool UnLocked() const noexcept { return m_lock == Lock_t::Unlocked; }

		std::mutex Mutex;
	private:
		enum class Lock_t { Locked = 0, Unlocked = 1 } m_lock{ Lock_t::Unlocked };
	};

	class CommandContext;
	class Texture;
	class ResourceStateTracker {
	public:
		ResourceStateTracker() noexcept;
		~ResourceStateTracker() = default;

		void ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier) noexcept;

		void TransitionResource(
			const Texture& texture,
			D3D12_RESOURCE_STATES stateAfter,
			uint32_t subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) noexcept;

		void TransitionResource(
			ID3D12Resource* resource,
			D3D12_RESOURCE_STATES stateAfter,
			uint32_t subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) noexcept;

		void UAVBarrier(const Texture* resource = nullptr) noexcept;
		void AliasBarrier(const Texture* resourceBefore = nullptr, const Texture* resourceAfter = nullptr) noexcept;

		uint32_t FlushPendingResourceBarriers(const CommandContext* context);
		void FlushResourceBarriers(const CommandContext* context);

		void CommitFinalResourceStates();
		void Reset() noexcept;

		static void Lock() noexcept;
		static void Unlock() noexcept;

		static void AddGlobalResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state) noexcept;
	private:
		// Tracks the state of a particular resource and all of its subresources.
		struct ResourceState {
			explicit ResourceState(D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON)
				:
				State(state)
			{}

			void SetSubResource(uint32_t subresource, D3D12_RESOURCE_STATES state) {
				if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) {
					State = state;
					SubresourceState.clear();
				}
				else {
					SubresourceState[subresource] = state;
				}
			}

			[[nodiscard]] D3D12_RESOURCE_STATES GetSubresourceState(uint32_t subresource) const {
				D3D12_RESOURCE_STATES state = State;
				const auto iter = SubresourceState.find(subresource);

				if (iter != SubresourceState.end()) {
					state = iter->second;
				}
				return state;
			}

			D3D12_RESOURCE_STATES State;
			std::map<UINT, D3D12_RESOURCE_STATES> SubresourceState;
		};

		std::vector<D3D12_RESOURCE_BARRIER> m_pendingResourceBarriers;
		std::vector<D3D12_RESOURCE_BARRIER> m_resourceBarriers;

		// The final (last known state) of the resources within a command list.
	    // The final resource state is committed to the global resource state when the 
	    // command list is closed but before it is executed on the command queue.
		std::unordered_map<ID3D12Resource*, ResourceState> m_finalResourceState;

		// The global resource state array stores the state of a resource
	    // between command list execution.
		static std::unordered_map<ID3D12Resource*, ResourceState> m_globalResourceState;

		static MutexLock m_lock;
	};
}