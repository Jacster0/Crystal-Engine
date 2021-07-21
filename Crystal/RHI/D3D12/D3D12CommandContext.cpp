#include "D3D12CommandContext.h"
#include "D3D12Buffer.h"
#include "D3D12Core.h"
#include "D3D12DynamicDescriptorHeap.h"
#include "D3D12Texture.h"
#include "D3D12PipelineState.h"
#include "D3D12RootSignature.h"
#include "D3D12RenderTarget.h"
#include "LinearAllocator.h"

#include "Utils/D3D12Exception.h"
#include "Utils/ResourceStateTracker.h"

#include "../Core/Logging/Logger.h"

#include <algorithm>

using namespace Crystal;
using namespace Microsoft::WRL;
namespace ranges = std::ranges;

CommandContext::CommandContext(CommandListType cmdListType) {
	auto& device = RHICore::GetDevice();

	ThrowIfFailed(device.CreateCommandAllocator(cmdListType.As<D3D12_COMMAND_LIST_TYPE>(), IID_PPV_ARGS(&m_commandAllocator)));
	ThrowIfFailed(device.CreateCommandList(
		0,
		cmdListType.As<D3D12_COMMAND_LIST_TYPE>(),
		m_commandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&m_commandList)));

	m_linearAllocator      = std::make_unique<LinearAllocator>();
	m_resourceStateTracker = std::make_unique<ResourceStateTracker>();

	for (auto i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++) {
		m_dynamicDescriptorHeap[i] = std::make_unique<DynamicDescriptorHeap>(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
		m_descriptorHeaps[i]       = nullptr;
	}
}

void CommandContext::TransitionResource(
	const Texture* const resource, 
	const TransitionBarrierSpecification& specification) const noexcept
{
	TransitionResource(resource->GetUnderlyingResource(), specification);
}

void CommandContext::CopyResource(const Texture& source, const Texture& dest) noexcept {
	const auto d3d12SourceResource = source.GetUnderlyingResource();
	const auto d3d12DestResource   = dest.GetUnderlyingResource();

	TransitionResource(d3d12DestResource, { {ResourceState_t::copy_dest} });
	TransitionResource(d3d12SourceResource, { { ResourceState_t::copy_source } });

	m_commandList->CopyResource(d3d12DestResource.Get(), d3d12SourceResource.Get());

	TrackResource(d3d12DestResource);
	TrackResource(d3d12SourceResource);
}

void CommandContext::ResolveSubResource(
	const Texture* const source, 
	const Texture* const dest, 
	uint32_t sourceSubResource, 
	uint32_t destSubResource)
{
	if (source && dest) {
		TransitionResource(dest, { { ResourceState_t::resolve_dest }, false, destSubResource });
		TransitionResource(source, { { ResourceState_t::resolve_source }, false, destSubResource });

		m_resourceStateTracker->FlushResourceBarriers(this);

		m_commandList->ResolveSubresource(
			dest->GetUnderlyingResource().Get(),
			destSubResource,
			source->GetUnderlyingResource().Get(),
			sourceSubResource,
			dest->GetResourceDesc().Format);

		TrackResource(source->GetUnderlyingResource());
		TrackResource(dest->GetUnderlyingResource());
	}
	else {
		crylog_info("Cannot resolve subresource because source or dest is null");
	}
}

void CommandContext::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, ID3D12DescriptorHeap* heap) noexcept {
	if (m_descriptorHeaps[heapType] != heap) {
		m_descriptorHeaps[heapType] = heap;
		
		const auto descriptorHeapCount = ranges::count_if(m_descriptorHeaps, 
			[](ID3D12DescriptorHeap* heap) { 
				return heap != nullptr; 
			}
		);

		m_commandList->SetDescriptorHeaps(descriptorHeapCount, m_descriptorHeaps.data());
	}
}

void CommandContext::SetPipelineState(const PipelineState* const pipelineState) noexcept {
	if (pipelineState) {
		const auto d3d12PipelineState = pipelineState->GetNativePipelineState().Get();

		if (m_pipeLineState != d3d12PipelineState) {
			m_pipeLineState = d3d12PipelineState;

			m_commandList->SetPipelineState(d3d12PipelineState);
			TrackResource(d3d12PipelineState);
		}
	}
	else {
		crylog_error("PipelineState cannot be null");
	}
}

bool CommandContext::Close(const CommandContext* const pendingCmdList) const noexcept {
	Close();

	auto numPendingBarriers = m_resourceStateTracker->FlushPendingResourceBarriers(pendingCmdList);
	m_resourceStateTracker->CommitFinalResourceStates();

	return numPendingBarriers > 0;
}

void CommandContext::Reset() {
	ThrowIfFailed(m_commandAllocator->Reset());
	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

	m_linearAllocator->Reset();

	for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++) {
		m_descriptorHeaps[i] = nullptr;
	}
}

void CommandContext::Close() const noexcept {
	m_resourceStateTracker->FlushResourceBarriers(this);
	m_commandList->Close();
}

void CommandContext::TrackResource(const ComPtr<ID3D12Object> object) noexcept {
	m_trackedObjects.push_back(object);
}

void CommandContext::ReleaseTrackedObjects() noexcept {
	m_trackedObjects.clear();
}

void CommandContext::InsertUAVBarrier(const Texture& resource, bool flushImmediate) const noexcept {
	const auto barrier = CD3DX12_RESOURCE_BARRIER::UAV(resource.GetUnderlyingResource().Get());

	m_resourceStateTracker->ResourceBarrier(barrier);

	if (flushImmediate) {
		m_resourceStateTracker->FlushResourceBarriers(this);
	}
}

void Crystal::CommandContext::InsertAliasingBarrier(const Texture& before, const Texture& after, bool flushImmediate) const noexcept  {
	const auto barrier = CD3DX12_RESOURCE_BARRIER::Aliasing(before.GetUnderlyingResource().Get(), after.GetUnderlyingResource().Get());
	m_resourceStateTracker->ResourceBarrier(barrier);

	if (flushImmediate) {
		m_resourceStateTracker->FlushResourceBarriers(this);
	}
}

void CommandContext::TransitionResource(
	ComPtr<ID3D12Resource> resource, 
	const TransitionBarrierSpecification& specification) const noexcept
{
	if (resource) {
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			resource.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			specification.NewState.As<D3D12_RESOURCE_STATES>(),
			specification.Subresource);

		m_resourceStateTracker->ResourceBarrier(barrier);
	}

	if (specification.FlushBarriers) {
		m_resourceStateTracker->FlushResourceBarriers(this);
	}
}

void ComputeContext::SetComputeRootSignature(const RootSignature* const rootSignature) noexcept {
	if (rootSignature) {
		const auto d3d12RootSignature = rootSignature->GetRootSignature().Get();

		if (m_rootSignature != d3d12RootSignature) {
			m_rootSignature = d3d12RootSignature;

			for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++) {
				m_dynamicDescriptorHeap[i]->ParseRootSignature(rootSignature);
			}

			m_commandList->SetComputeRootSignature(m_rootSignature);

			TrackResource(m_rootSignature);
		}
	}
	else {
		crylog_error("RootSignature cannot be null");
	}
}

void ComputeContext::Dispatch(uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ) noexcept {
	m_resourceStateTracker->FlushResourceBarriers(this);

	for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++) {
		m_dynamicDescriptorHeap[i]->BindDescriptorsForDispatch(*this);
	}
	m_commandList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);
}

void ComputeContext::SetComputeShaderConstants(
	uint32_t rootParameterIndex, 
	uint32_t numConstants, 
	const void* const constants) const noexcept 
{
	m_commandList->SetComputeRoot32BitConstants(rootParameterIndex, numConstants, constants, 0);
}

void GraphicsContext::ClearRTV(const Texture& texture, const float* const clearColor) noexcept {
	TransitionResource(&texture, { { ResourceState_t::render_target }, true });
	m_commandList->ClearRenderTargetView(texture.GetRenderTargetView(), clearColor, 0, nullptr);
	TrackResource(texture.GetUnderlyingResource());
}

void GraphicsContext::ClearDSV(const Texture& texture, ClearFlag clearFlags, float depth, uint8_t stencil) noexcept {
	TransitionResource(&texture, { { ResourceState_t::depth_write }, true });
	m_commandList->ClearDepthStencilView(texture.GetDepthStencilView(), clearFlags.As<D3D12_CLEAR_FLAGS>(), depth, stencil, 0, nullptr);
	TrackResource(texture.GetUnderlyingResource());
}

void GraphicsContext::SetGraphicsRootSignature(const RootSignature const* rootSignature) noexcept {
	if (rootSignature) {
		const auto d3d12RootSignature = rootSignature->GetRootSignature().Get();

		if (m_rootSignature != d3d12RootSignature) {
			m_rootSignature = d3d12RootSignature;

			for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++) {
				m_dynamicDescriptorHeap[i]->ParseRootSignature(rootSignature);
			}

			m_commandList->SetGraphicsRootSignature(m_rootSignature);

			TrackResource(m_rootSignature);
		}
	}
	else {
		crylog_error("RootSignature cannot be null");
	}
}

void GraphicsContext::SetGraphicsShaderConstants(
	uint32_t rootParameterIndex, 
	uint32_t numConstants, 
	const void* const constants) const noexcept
{
	m_commandList->SetGraphicsRoot32BitConstants(rootParameterIndex, numConstants, constants, 0);
}

void GraphicsContext::SetConstantBuffer(
	uint32_t rootParameterIndex, 
	size_t sizeInBytes, 
	const void* bufferData) const noexcept
{
	const auto alloc = m_linearAllocator->Allocate(sizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	std::memcpy(alloc.CPU, bufferData, sizeInBytes);

	m_commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, alloc.GPU);
}

void GraphicsContext::SetShaderResourceView(
	uint32_t slot, 
	size_t numElements, 
	size_t elementSize, 
	const void* bufferData) const noexcept
{
	const size_t bufferSize = numElements * elementSize;
	const auto heapAlloc    = m_linearAllocator->Allocate(bufferSize, elementSize);

	std::memcpy(heapAlloc.CPU, bufferData, bufferSize);

	m_commandList->SetGraphicsRootShaderResourceView(slot, heapAlloc.GPU);
}

void GraphicsContext::SetScissorRect(const Math::Rectangle& scissorRect) const noexcept {
	SetScissorRects(&scissorRect, 1);
}

void GraphicsContext::SetScissorRects(const Math::Rectangle* const scissorRects, const uint32_t scissorCount) const noexcept {
	m_commandList->RSSetScissorRects(scissorCount, reinterpret_cast<const D3D12_RECT* const>(scissorRects));
}

void GraphicsContext::SetViewport(const Viewport& viewport) const noexcept {
	SetViewports(&viewport, 1);
}

void GraphicsContext::SetViewports(const Viewport* const viewports, const uint32_t viewportCount) const noexcept {
	m_commandList->RSSetViewports(viewportCount, reinterpret_cast<const D3D12_VIEWPORT* const>(viewports));
}

void GraphicsContext::SetRenderTarget(const RenderTarget& renderTarget) noexcept {
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> renderTargetDescriptors(AttachmentPoint::NumAttachmentPoints);

	const auto& textures = renderTarget.GetTextures();

	//Bind color targets (max of 8 render targets can be bound to the rendering pipeline
	for (size_t i = 0; i < 8; i++) {
		auto texture = textures[i];

		if (texture) {
			TransitionResource(texture.get(), { {ResourceState_t::render_target} });
			renderTargetDescriptors.push_back(texture->GetRenderTargetView());

			TrackResource(texture->GetUnderlyingResource());
		}
	}

	const auto depthTexture = renderTarget.GetTexture(AttachmentPoint::DepthStencil);
	CD3DX12_CPU_DESCRIPTOR_HANDLE depthStencilDescriptor(D3D12_DEFAULT);

	if(depthTexture) {
		TransitionResource(depthTexture.get(), { {ResourceState_t::depth_write} });
		depthStencilDescriptor = depthTexture->GetDepthStencilView();

		TrackResource(depthTexture->GetUnderlyingResource());
	}

	m_commandList->OMSetRenderTargets(
		static_cast<uint32_t>(renderTargetDescriptors.size()),
		renderTargetDescriptors.data(),
		false,
		&depthStencilDescriptor);
}

void GraphicsContext::SetPrimitiveTopology(PrimitiveTopology topology) noexcept {
	if (m_primitiveTopology.Topology != topology.Topology) {
		m_primitiveTopology.Topology = topology.Topology;
		m_commandList->IASetPrimitiveTopology(topology.As<D3D_PRIMITIVE_TOPOLOGY>());
	}
}

const PrimitiveTopology& GraphicsContext::GetPrimitiveTopology() const noexcept {
	return m_primitiveTopology;
}

void GraphicsContext::SetVertexBuffer(uint32_t slot, const Buffer* const vertexBuffer) noexcept {
	SetVertexBuffers(slot, { vertexBuffer });
}

void GraphicsContext::SetVertexBuffers(uint32_t startSlot, const std::vector<const Buffer*>& vertexBuffers) noexcept {
	std::vector<D3D12_VERTEX_BUFFER_VIEW> vbViews(vertexBuffers.size());

	for (const auto vertexBuffer : vertexBuffers) {
		if (vertexBuffer) {
			TransitionResource(vertexBuffer->GetResource(), { { ResourceState_t::vertex_and_constant_buffer } });
			TrackResource(vertexBuffer->GetResource());

			vbViews.push_back({ vertexBuffer->BufferLocation(), vertexBuffer->Size(), vertexBuffer->Stride() });
		}
	}
	m_commandList->IASetVertexBuffers(startSlot, vbViews.size(), vbViews.data());
}

void GraphicsContext::SetDynamicVertexBuffer(uint32_t slot, size_t numVertices, size_t vertexSize, const void* vertexBufferData) {
	const size_t bufferSize = numVertices * vertexSize;
	const auto heapAlloc    = m_linearAllocator->Allocate(bufferSize, vertexSize);

	std::memcpy(heapAlloc.CPU, vertexBufferData, bufferSize);

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{
		.BufferLocation = heapAlloc.GPU,
		.SizeInBytes    = static_cast<uint32_t>(bufferSize),
		.StrideInBytes  = static_cast<uint32_t>(vertexSize)
	};

	m_commandList->IASetVertexBuffers(slot, 1, &vertexBufferView);
}

void GraphicsContext::SetIndexBuffer(const Buffer* const indexBuffer) noexcept {
	if (indexBuffer) {
		TransitionResource(indexBuffer->GetResource(), { { ResourceState_t::index_buffer } });
		TrackResource(indexBuffer->GetResource());

		const D3D12_INDEX_BUFFER_VIEW ibv{
			.BufferLocation = indexBuffer->BufferLocation(),
			.SizeInBytes    = indexBuffer->Size(),
			.Format         = static_cast<DXGI_FORMAT>(indexBuffer->GetDesc().Format)
		};

		m_commandList->IASetIndexBuffer(&ibv);
	}
}

void GraphicsContext::SetDynamicIndexBuffer(size_t numIndicies, IndexFormat_t indexFormat, const void* indexBufferData) {
	const size_t indexSizeInBytes = indexFormat == IndexFormat_t::uint_16 ? 2 : 4;
	size_t bufferSize             = numIndicies * indexSizeInBytes;
	const auto heapAlloc          = m_linearAllocator->Allocate(bufferSize, indexSizeInBytes);

	std::memcpy(heapAlloc.CPU, indexBufferData, bufferSize);

	D3D12_INDEX_BUFFER_VIEW indexBufferView{
		.BufferLocation = heapAlloc.GPU,
		.SizeInBytes    = static_cast<DWORD>(bufferSize),
		.Format         = static_cast<DXGI_FORMAT>(indexFormat)
	};

	m_commandList->IASetIndexBuffer(&indexBufferView);
}

void GraphicsContext::Draw(
	uint32_t vertexCount, 
	uint32_t instanceCount, 
	uint32_t startVertex, 
	uint32_t startInstance) noexcept
{
	m_resourceStateTracker->FlushResourceBarriers(this);

	for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++) {
		m_dynamicDescriptorHeap[i]->BindDescriptorsForDraw(*this);
	}
	m_commandList->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
}

void GraphicsContext::DrawIndexed(
	uint32_t indexCount, 
	uint32_t instanceCount, 
	uint32_t startIndex, 
	int32_t baseVertex, 
	uint32_t startInstance) noexcept
{
	m_resourceStateTracker->FlushResourceBarriers(this);

	for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++) {
		m_dynamicDescriptorHeap[i]->BindDescriptorsForDraw(*this);
	}
	m_commandList->DrawIndexedInstanced(indexCount, instanceCount, startIndex,baseVertex,startInstance);
}
