#pragma once

#include "Utils/d3dx12.h"
#include <wrl.h>
#include <array>

namespace Crystal {
	class RootSignature {
	public:
		 RootSignature();
		~RootSignature();

		void Intialize(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc);
		void Destroy();

		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const noexcept { return m_rootSignature; }
		[[nodiscard]] uint32_t GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) const;
		[[nodiscard]] uint32_t GetNumDescriptors(uint32_t rootIndex) const noexcept { return m_numDescriptorsPerTable.at(rootIndex); }
	private:
		void SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc);

		D3D12_ROOT_SIGNATURE_DESC1 m_rootSignatureDesc{};
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		std::array<uint32_t, 32> m_numDescriptorsPerTable{};
		int32_t m_samplerBitMask{};
		uint32_t m_descriptorTableBitMask{};
	};
}