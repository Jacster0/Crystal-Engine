#include "D3D12Texture.h"
#include "D3D12Core.h"

#include "Utils/d3dx12.h"
#include "Utils/D3D12Exception.h"

#include <algorithm>

using namespace Crystal;

Texture::Texture(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* const clearValue)
	:
	m_resourceDesc(resourceDesc)
{
	auto& device = RHICore::get_device();

	if (clearValue) {
		m_clearValue = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
	}

	const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	ThrowIfFailed(device.CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		m_clearValue.get(),
		IID_PPV_ARGS(&m_resource)));

	//Todo: add resourcestate to our resourcestate tracker
	SetTextureType();
	CheckFeatureSupport();
	CreateViews();
}

Texture::Texture(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* const clearValue)
	:
	m_resource(resource),
	m_resourceDesc(resource->GetDesc())
{
	if (clearValue) {
		m_clearValue = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
	}

	SetTextureType();
	CheckFeatureSupport();
	CreateViews();
}

void Texture::Resize(uint32_t width, uint32_t height, uint32_t depthOrArraySize) {
	if (m_resource) {
		CD3DX12_RESOURCE_DESC resourceDesc(m_resource->GetDesc());

		resourceDesc.Width            = std::max(width, 1u);
		resourceDesc.Height           = std::max(height, 1u);
		resourceDesc.DepthOrArraySize = depthOrArraySize;
		resourceDesc.MipLevels        = resourceDesc.SampleDesc.Count > 1 ? 1 : 0;

		auto& device = RHICore::get_device();
		const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		ThrowIfFailed(device.CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			m_clearValue.get(),
			IID_PPV_ARGS(&m_resource)));

		m_resource->SetName(m_textureName.c_str());

		//Todo: add resourcestate to our resourcestate tracker
		CreateViews();
	}
}

D3D12_RESOURCE_DESC Texture::GetResourceDesc() const noexcept {
	return m_resourceDesc;
}

void Texture::SetName(std::wstring_view name) noexcept {
	m_textureName = name;

	if (m_resource && !m_textureName.empty()) {
		m_resource->SetName(m_textureName.c_str());
	}
}

std::wstring Texture::GetName() const noexcept {
	return m_textureName;
}

void Texture::SetTextureType() noexcept {
	switch (m_resourceDesc.Dimension) {
	case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
		m_textureType = TextureType::Texture1D;
		break;
	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
		m_textureType = TextureType::Texture2D;
		break;
	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
		m_textureType = TextureType::Texture3D;
		break;
	}
}

bool Texture::CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const noexcept {
	return (m_formatSupport.Support1 & formatSupport) != 0;
}

bool Texture::CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const noexcept {
	return (m_formatSupport.Support2 & formatSupport) != 0;
}

bool Texture::CheckSRVSupport() const noexcept {
	return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE);
}

bool Texture::CheckRTVSupport() const noexcept {
	return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
}

bool Texture::CheckDSVSupport() const noexcept {
	return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);
}

bool Texture::CheckUAVSupport() const noexcept {
	return
		CheckFormatSupport(D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) &&
		CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) &&
		CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE);
}


void Texture::CreateViews() noexcept {
	if (m_resource) {
		auto& device = RHICore::get_device();
		CD3DX12_RESOURCE_DESC resourceDesc(m_resource->GetDesc());

		//RTV
		if ((resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 && CheckRTVSupport()) {
			m_renderTargetView = RHICore::allocate_descriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			device.CreateRenderTargetView(m_resource.Get(), nullptr, m_renderTargetView.GetDescriptorHandle());
		}

		//DSV
		if ((resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 && CheckDSVSupport()) {
			m_depthStencilView = RHICore::allocate_descriptors(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			device.CreateDepthStencilView(m_resource.Get(), nullptr, m_depthStencilView.GetDescriptorHandle());
		}

		//SRV
		if ((resourceDesc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) != 0 && CheckSRVSupport()) {
			m_shaderResourceView = RHICore::allocate_descriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			device.CreateShaderResourceView(m_resource.Get(), nullptr, m_shaderResourceView.GetDescriptorHandle());
		}

		//Create an UAV for each mip (3D textures not supported)
		if ((resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) != 0 && CheckUAVSupport() && resourceDesc.DepthOrArraySize == 1) {
			m_unorderedAccessView = RHICore::allocate_descriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, resourceDesc.MipLevels);

			for (uint32_t i = 0; i < resourceDesc.MipLevels; i++) {
				const auto& uavDesc = GetUAVDesc(resourceDesc, i);

				device.CreateUnorderedAccessView(
					m_resource.Get(),
					nullptr,
					&uavDesc,
					m_unorderedAccessView.GetDescriptorHandle(i));
			}
		}
	}
}

void Texture::CheckFeatureSupport() {
	m_formatSupport.Format = m_resource->GetDesc().Format;

	auto& device = RHICore::get_device();
	ThrowIfFailed(device.CheckFeatureSupport(
		D3D12_FEATURE_FORMAT_SUPPORT,
		&m_formatSupport,
		sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT)));
}

D3D12_UNORDERED_ACCESS_VIEW_DESC Texture::GetUAVDesc(
	const D3D12_RESOURCE_DESC& resourceDesc, 
	uint32_t mipSlice, 
	uint32_t arraySlice, 
	uint32_t planeSlice)
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = { .Format = resourceDesc.Format };

	switch (resourceDesc.Dimension) {
	case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
		if (resourceDesc.DepthOrArraySize > 1) {
			uavDesc.ViewDimension                  = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
			uavDesc.Texture1DArray.ArraySize       = resourceDesc.DepthOrArraySize - arraySlice;
			uavDesc.Texture1DArray.FirstArraySlice = arraySlice;
			uavDesc.Texture1DArray.MipSlice        = mipSlice;
		}
		else {
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
			uavDesc.Texture1D.MipSlice = mipSlice;
		}
		break;
	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
		if (resourceDesc.DepthOrArraySize > 1) {
			uavDesc.ViewDimension                  = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			uavDesc.Texture2DArray.ArraySize       = resourceDesc.DepthOrArraySize - arraySlice;
			uavDesc.Texture2DArray.FirstArraySlice = arraySlice;
			uavDesc.Texture2DArray.PlaneSlice      = planeSlice;
			uavDesc.Texture2DArray.MipSlice        = mipSlice;
		}
		else {
			uavDesc.ViewDimension        = D3D12_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.PlaneSlice = planeSlice;
			uavDesc.Texture2D.MipSlice   = mipSlice;
		}
		break;
	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
		uavDesc.ViewDimension         = D3D12_UAV_DIMENSION_TEXTURE3D;
		uavDesc.Texture3D.WSize       = resourceDesc.DepthOrArraySize - arraySlice;
		uavDesc.Texture3D.FirstWSlice = arraySlice;
		uavDesc.Texture3D.MipSlice    = mipSlice;
		break;
	default:
		throw std::exception("Invalid resource dimension");
	}

	return uavDesc;
}

uint32_t Texture::Width() const noexcept {
	return m_resourceDesc.Width;
}

uint32_t Texture::Height() const noexcept {
	return m_resourceDesc.Height; 
}

uint32_t Crystal::Texture::MipmapCount() const noexcept {
	return m_resourceDesc.MipLevels;
}

TextureType Texture::GetTextureType() const noexcept {
	return m_textureType;
}

bool Texture::IsMultiSampled() const noexcept {
	return m_resourceDesc.SampleDesc.Count > 1;
}

Microsoft::WRL::ComPtr<ID3D12Resource> Texture::GetUnderlyingResource() const noexcept {
	return m_resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetRenderTargetView() const
{
	return m_renderTargetView.GetDescriptorHandle(); 
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetDepthStencilView() const {
	return m_depthStencilView.GetDescriptorHandle();
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetShaderResourceView() const {
	return m_shaderResourceView.GetDescriptorHandle();
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetUnorderedAccessView(uint32_t mipLevel) const {
	return m_unorderedAccessView.GetDescriptorHandle(mipLevel);
}
