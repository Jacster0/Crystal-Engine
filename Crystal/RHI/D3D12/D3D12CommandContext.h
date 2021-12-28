#pragma once
#include <memory>
#include <array>
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <span>

#include "../Graphics/Types/Types.h"
#include "../Core/Math/Rectangle.h"

namespace Crystal {
	class Buffer;
	class DynamicDescriptorHeap;
	class LinearAllocator;
	class PipelineState;
	class ResourceStateTracker;
	class RootSignature;
	class RenderTarget;
	class Texture;
	class Viewport;
	class CommandContext {
	public:
		explicit CommandContext(CommandListType cmdListType);

		CommandContext(const CommandContext&)            = delete;
		CommandContext& operator=(const CommandContext&) = delete;
		CommandContext(CommandContext&&)                 = delete;
		CommandContext& operator=(CommandContext&&)      = delete;

		virtual ~CommandContext() = default;

		void TransitionResource(
			const Texture* resource,
			const TransitionBarrierSpecification& specification) const noexcept;
		void CopyResource(const Texture& source, const Texture& dest) noexcept;
		void ResolveSubResource(
			const Texture* source,
			const Texture* dest,
			uint32_t sourceSubResource = 0,
			uint32_t destSubResource = 0);
		
		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> GetNativeCommandList() const noexcept { return m_d3d12CommandList; }

		void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, ID3D12DescriptorHeap* heap) noexcept;
		void SetPipelineState(const PipelineState* pipelineState) noexcept;

		bool Close(const CommandContext* pendingCmdList) const noexcept;
		void Reset();

		[[nodiscard]] class GraphicsContext& AsGraphicsContext() noexcept;
		[[nodiscard]] class ComputeContext& AsComputeContext() noexcept;
		[[nodiscard]] class RayTracingContext& AsRayTracingContext() noexcept;
	protected:
		void Close() const noexcept;

		void TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> object) noexcept;
		void ReleaseTrackedObjects() noexcept;

		void InsertUAVBarrier(const Texture& resource, bool flushImmediate = false) const noexcept;
		void InsertAliasingBarrier(const Texture& before, const Texture& after, bool flushImmediate = false) const noexcept;
		void TransitionResource(
			Microsoft::WRL::ComPtr<ID3D12Resource> resource,
			const TransitionBarrierSpecification& specification) const noexcept;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_d3d12CommandList;

		std::unique_ptr<LinearAllocator> m_linearAllocator;
		std::unique_ptr<ResourceStateTracker> m_resourceStateTracker;

		std::array<std::unique_ptr<DynamicDescriptorHeap>, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> m_dynamicDescriptorHeap;
		std::array<ID3D12DescriptorHeap*, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> m_descriptorHeaps;

		std::vector<Microsoft::WRL::ComPtr<ID3D12Object>> m_trackedObjects;

		ID3D12PipelineState* m_pipeLineState{ nullptr };
		ID3D12RootSignature* m_rootSignature{ nullptr };
	};

	
	class ComputeContext : public CommandContext {
	public:
		explicit ComputeContext(CommandListType cmdListType);

		void SetComputeRootSignature(const RootSignature* rootSignature) noexcept;
		void Dispatch(uint32_t numGroupsX, uint32_t numGroupsY = 1, uint32_t numGroupsZ = 1) noexcept;
		void SetComputeShaderConstants(uint32_t rootParameterIndex, uint32_t numConstants, const void* constants) const noexcept;
	};

	class GraphicsContext : public ComputeContext {
	public:
		explicit GraphicsContext(CommandListType cmdListType);

		void ClearRTV(const Texture& texture, const float* clearColor) noexcept;
		void ClearDSV(const Texture& texture, ClearFlag clearFlags, float depth = 1.0f, uint8_t stencil = 0) noexcept;

		void SetGraphicsRootSignature(const RootSignature* rootSignature) noexcept;
		void SetGraphicsShaderConstants(uint32_t rootParameterIndex, uint32_t numConstants, const void* constants) const noexcept;
		void SetConstantBuffer(uint32_t rootParameterIndex, size_t sizeInBytes, const void* bufferData) const noexcept;
		void SetShaderResourceView(uint32_t slot, size_t numElements, size_t elementSize, const void* bufferData) const noexcept;

		void SetScissorRect(const Math::Rectangle& scissorRect) const noexcept;
		void SetScissorRects(std::span<const Math::Rectangle> scissorRects) const noexcept;
		void SetViewport(const Viewport& viewport) const noexcept;
		void SetViewports(std::span<const Viewport> viewports) const noexcept;

		void SetRenderTarget(const RenderTarget& renderTarget) noexcept;
		void SetPrimitiveTopology(PrimitiveTopology topology) noexcept;
		[[nodiscard]] const PrimitiveTopology& GetPrimitiveTopology() const noexcept;

		void SetVertexBuffer(uint32_t slot, const Buffer* vertexBuffer) noexcept;
		void SetVertexBuffers(uint32_t startSlot, const std::vector<const Buffer*>& vertexBuffers) noexcept;
		void SetDynamicVertexBuffer(uint32_t slot, size_t numVertices, size_t vertexSize, const void* vertexBufferData) const;

		void SetIndexBuffer(const Buffer* indexBuffer) noexcept;
		void SetDynamicIndexBuffer(size_t numIndicies, IndexFormat_t indexFormat, const void* indexBufferData) const;

		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t startVertex = 0, uint32_t startInstance = 0) noexcept;
		void DrawIndexed(
			uint32_t indexCount,
			uint32_t instanceCount = 1,
			uint32_t startIndex = 0,
			int32_t  baseVertex = 0,
			uint32_t startInstance = 0) noexcept;
	private:
		PrimitiveTopology m_primitiveTopology;
	};

	class RayTracingContext : public GraphicsContext {};

	inline GraphicsContext&   CommandContext::AsGraphicsContext()   noexcept { return static_cast<GraphicsContext&>(*this); }
	inline ComputeContext&    CommandContext::AsComputeContext()    noexcept { return static_cast<ComputeContext&>(*this); }
	inline RayTracingContext& CommandContext::AsRayTracingContext() noexcept { return static_cast<RayTracingContext&>(*this); }
}