#pragma once
#include "Platform/Windows/CrystalWindow.h"
#include "D3D12DescriptorHeap.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <memory>

namespace Crystal {
	enum class TextureType {
		Texture1D,
		Texture2D,
		Texture3D,
	};

	class Texture {
	public:
		Texture(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue = nullptr);
		Texture(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clearValue = nullptr);

		void Resize(uint32_t width, uint32_t height, uint32_t depthOrArraySize = 1);
		[[nodiscard]] uint32_t Width() const noexcept;
		[[nodiscard]] uint32_t Height() const noexcept; 
		[[nodiscard]] uint32_t MipmapCount() const noexcept; 
		[[nodiscard]] TextureType GetTextureType() const noexcept;
		[[nodiscard]] bool IsMultiSampled() const noexcept;

		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> GetUnderlyingResource() const noexcept;
		[[nodiscard]] D3D12_RESOURCE_DESC GetResourceDesc() const noexcept;

		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const; 
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const; 
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const; 
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(uint32_t mipLevel) const;

		void SetName(std::wstring_view name) noexcept;
		[[nodiscard]] std::wstring GetName() const noexcept;
	private:
		void SetTextureType() noexcept;

		[[nodiscard]] bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const noexcept;
		[[nodiscard]] bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const noexcept;

		[[nodiscard]] bool CheckSRVSupport() const noexcept;
		[[nodiscard]] bool CheckRTVSupport() const noexcept; 
		[[nodiscard]] bool CheckDSVSupport() const noexcept;
		[[nodiscard]] bool CheckUAVSupport() const noexcept; 

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

		D3D12_RESOURCE_DESC m_resourceDesc;
		TextureType m_textureType;
	};
}
