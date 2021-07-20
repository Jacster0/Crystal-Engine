#pragma once
#include "Utils/d3dx12.h"
#include <wrl.h>
#include <memory>
#include <queue>
#include <functional>
#include <array>

namespace Crystal {
	using SetDescriptorTableCB = std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)>;

	class CommandContext;
	class RootSignature;

	struct DescriptorTableCache {

		void Reset() {
			NumDescriptors = 0;
			BaseDescriptor = nullptr;
		}

		uint32_t NumDescriptors{0};
		D3D12_CPU_DESCRIPTOR_HANDLE* BaseDescriptor{nullptr};
	};

	class DynamicDescriptorHeap {
	public:
		DynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptorsPerHeap = 1024) noexcept;
		void StageDescriptors(
			const uint32_t rootParameterIndex,
			const uint32_t offset,
			const uint32_t numDescriptors,
			const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptors
		);
		void BindDescriptorsForDraw(CommandContext& context) noexcept;
		void BindDescriptorsForDispatch(CommandContext& context) noexcept;

		const D3D12_GPU_DESCRIPTOR_HANDLE CopyDescriptor(
			CommandContext& commandList,  
			D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor) noexcept;

		void StageInlineCBV(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferAddr) noexcept;
		void StageInlineSRV(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferAddr) noexcept;
		void StageInlineUAV(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferAddr) noexcept;

		void ParseRootSignature(const RootSignature* const rootSignature) noexcept;
		void Reset() noexcept;
	private:
		//Binds the descriptors to the command list. 
		void Bind(
			CommandContext& context,
			SetDescriptorTableCB setDescriptorTable) noexcept;
		//Request a descriptor heap if one is available.
		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RequestDescriptorHeap();
		//Create a new descriptor heap if no descriptor heap is available.
		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap();

		//Get the number of stale descriptors that need to be copied
		//to GPU visible descriptor heap.
		[[nodiscard]] constexpr uint32_t GetStaleDescriptorCount() const noexcept;

		//Max number of descriptor tables per root signature
		static constexpr uint32_t MAX_DESCRIPTOR_TABLES = 32;

		D3D12_DESCRIPTOR_HEAP_TYPE m_descriptorHeapType;
		uint32_t m_numDescriptorsPerHeap;
		uint32_t m_descriptorHandleIncrementSize;

		uint32_t m_descriptorTableBitMask;
		uint32_t m_staleDescriptorTableBitMask;
		uint32_t m_staleCBVBitMask;
		uint32_t m_staleSRVBitMask;
		uint32_t m_staleUAVBitMask;

		std::array<D3D12_GPU_VIRTUAL_ADDRESS, MAX_DESCRIPTOR_TABLES> m_inlineCBV;
		std::array<D3D12_GPU_VIRTUAL_ADDRESS, MAX_DESCRIPTOR_TABLES> m_inlineSRV;
		std::array<D3D12_GPU_VIRTUAL_ADDRESS, MAX_DESCRIPTOR_TABLES> m_inlineUAV;

		std::array<DescriptorTableCache, MAX_DESCRIPTOR_TABLES> m_descriptorTableCache;
		std::unique_ptr<D3D12_CPU_DESCRIPTOR_HANDLE[]> m_descriptorHandleCache;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_currentDescriptorHeap;
		std::queue<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> m_descriptorHeapPool;
		std::queue<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> m_availableDescriptorHeaps;

		CD3DX12_GPU_DESCRIPTOR_HANDLE m_currentGPUDescriptorHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_currentCPUDescriptorHandle;

		uint32_t m_numFreeHandles;
	};
}