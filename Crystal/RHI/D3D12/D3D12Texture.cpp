#include "D3D12Texture.h"
#include "D3D12Core.h"

#include "Utils/d3dx12.h"
#include "Utils/D3D12Exception.h"

#include <algorithm>

using namespace Crystal;

Texture::Texture(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE const* clearValue) {
	auto& device = RHICore::GetDevice();

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
	CheckFeatureSupport();
	CreateViews();
}

Texture::Texture(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE const* clearValue)
	:
	m_resource(resource)
{
	if (clearValue) {
		m_clearValue = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
	}
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

		auto& device = RHICore::GetDevice();
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

D3D12_RESOURCE_DESC Crystal::Texture::GetResourceDesc() const noexcept {
	D3D12_RESOURCE_DESC resDesc = {};

	if (m_resource) {
		resDesc = m_resource->GetDesc();
	}

	return resDesc;
}

void Texture::SetName(const std::wstring_view name) noexcept {
	m_textureName = name;

	if (m_resource && !m_textureName.empty()) {
		m_resource->SetName(m_textureName.c_str());
	}
}

bool Texture::CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const noexcept {
	return (m_formatSupport.Support1 & formatSupport) != 0;
}

bool Texture::CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const noexcept {
	return (m_formatSupport.Support2 & formatSupport) != 0;
}

void Texture::CreateViews() noexcept {
	if (m_resource) {
		auto& device = RHICore::GetDevice();
		CD3DX12_RESOURCE_DESC resourceDesc(m_resource->GetDesc());

		//RTV
		if ((resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 && CheckRTVSupport()) {
			m_renderTargetView = RHICore::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			device.CreateRenderTargetView(m_resource.Get(), nullptr, m_renderTargetView.GetDescriptorHandle());
		}

		//DSV
		if ((resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 && CheckDSVSupport()) {
			m_depthStencilView = RHICore::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			device.CreateDepthStencilView(m_resource.Get(), nullptr, m_depthStencilView.GetDescriptorHandle());
		}

		//SRV
		if ((resourceDesc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) != 0 && CheckSRVSupport()) {
			m_shaderResourceView = RHICore::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			device.CreateShaderResourceView(m_resource.Get(), nullptr, m_shaderResourceView.GetDescriptorHandle());
		}

		//Create an UAV for each mip (3D textures not supported)
		if ((resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) != 0 && CheckUAVSupport() && resourceDesc.DepthOrArraySize == 1) {
			m_unorderedAccessView = RHICore::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, resourceDesc.MipLevels);

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

	auto& device = RHICore::GetDevice();
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
