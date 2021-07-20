#pragma once
#include "../Platform/Windows/CrystalWindow.h"
#include "D3D12DescriptorHeap.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <memory>

namespace Crystal {
	class Texture {
	public:
		Texture(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE const* clearValue = nullptr);
		Texture(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE const* clearValue = nullptr);

		void Resize(uint32_t width, uint32_t height, uint32_t depthOrArraySize = 1);

		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> GetUnderlyingResource() const noexcept { return m_resource; }
		[[nodiscard]] D3D12_RESOURCE_DESC GetResourceDesc() const noexcept;

		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const { return m_renderTargetView.GetDescriptorHandle(); }
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const { return m_depthStencilView.GetDescriptorHandle(); }
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const { return m_shaderResourceView.GetDescriptorHandle(); }
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(uint32_t mipLevel) const { return m_unorderedAccessView.GetDescriptorHandle(mipLevel); }

		void SetName(const std::wstring_view namme) noexcept;
		const [[nodiscard]] std::wstring& GetName() const noexcept { return m_textureName; }
	private:
		[[nodiscard]] bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const noexcept;
		[[nodiscard]] bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const noexcept;

		[[nodiscard]] bool CheckSRVSupport() const noexcept { return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE); }
		[[nodiscard]] bool CheckRTVSupport() const noexcept { return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_RENDER_TARGET); }
		[[nodiscard]] bool CheckDSVSupport() const noexcept { return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL); }
		[[nodiscard]] bool CheckUAVSupport() const noexcept {
			return
				CheckFormatSupport(D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) &&
				CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) &&
				CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE);
		}

		void CreateViews() noexcept;
		void CheckFeatureSupport();
		D3D12_UNORDERED_ACCESS_VIEW_DESC GetUAVDesc(
			const D3D12_RESOURCE_DESC& resourceDesc,
			uint32_t mipSlice,
			uint32_t arraySlice = 0,
			uint32_t planeSlice = 0);

		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource{ nullptr };
		std::wstring m_textureName{};
		std::unique_ptr<D3D12_CLEAR_VALUE> m_clearValue{ nullptr };
		D3D12_FEATURE_DATA_FORMAT_SUPPORT m_formatSupport{};

		DescriptorAllocation m_renderTargetView;
		DescriptorAllocation m_depthStencilView;
		DescriptorAllocation m_shaderResourceView;
		DescriptorAllocation m_unorderedAccessView;
	};
}
