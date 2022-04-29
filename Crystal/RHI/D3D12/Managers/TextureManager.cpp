#include "TextureManager.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/Utils/StringUtils.h"
#include "DirectXTex/DirectXTex.h"
#include "RHI/RHICore.h"
#include "RHI/D3D12/D3D12CommandContext.h"
#include "RHI/D3D12/Utils/D3D12Exception.h"
#include "../Utils/ResourceStateTracker.h"
#include <Crystal/ComputeMipsPass.h>
#include <range/v3/all.hpp>

using namespace Crystal;
using namespace DirectX;
using namespace Microsoft::WRL;
namespace rn = ranges;

namespace impl {
	ComPtr<ID3D12Resource> CreateD3D12Texture(const TexMetadata& metadata) {
		D3D12_RESOURCE_DESC textureDesc;

		switch (metadata.dimension) {
		case TEX_DIMENSION_TEXTURE1D:
			textureDesc = CD3DX12_RESOURCE_DESC::Tex1D(
				metadata.format,
				metadata.width,
				static_cast<uint16_t>(metadata.arraySize));
			break;
		case TEX_DIMENSION_TEXTURE2D:
			textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
				metadata.format,
				metadata.width,
				static_cast<uint32_t>(metadata.height),
				static_cast<uint16_t>(metadata.arraySize));
			break;
		case TEX_DIMENSION_TEXTURE3D:
			textureDesc = CD3DX12_RESOURCE_DESC::Tex3D(
				metadata.format,
				metadata.width,
				static_cast<uint32_t>(metadata.height),
				static_cast<uint16_t>(metadata.depth));
			break;
		default:
			throw std::exception("Invalid texture dimension");
		}

		auto& d3d12Device = RHICore::get_device();
		ComPtr<ID3D12Resource> textureResource;

		const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		ThrowIfFailed(d3d12Device.CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&textureResource)));

		return textureResource;
	}

	std::vector<D3D12_SUBRESOURCE_DATA> CreateSubResources(const ScratchImage& scratchImage) {
		std::vector<D3D12_SUBRESOURCE_DATA> subResources;
		subResources.reserve(scratchImage.GetImageCount());

		std::span images{ scratchImage.GetImages(), scratchImage.GetImageCount() };

		for (const auto& [subResource, image] : rn::views::zip(subResources, images)) {
			subResource.RowPitch   = image.rowPitch;
			subResource.SlicePitch = image.slicePitch;
			subResource.pData      = image.pixels;
		}
		return subResources;
	}

	std::unordered_map<std::wstring, ID3D12Resource*> textureCache;
	std::mutex textureCacheMutex;
}

std::unique_ptr<Texture> TextureManager::LoadTextureFromFile(CommandContext& ctx, std::string_view fileName, bool sRBG) {
	if (!FileSystem::Exists(fileName)) [[likely]] {
		throw std::exception("File not found");
	}

	const auto wideFileName = StringConverter::To<std::wstring>(fileName);

	/*std::scoped_lock(impl::textureCacheMutex);*/

	if(impl::textureCache.contains(wideFileName)) {
		return std::make_unique<Texture>(impl::textureCache[wideFileName]);
	}

	TexMetadata metadata{};
	ScratchImage scratchImage;

	const auto fileExtensions = FileSystem::GetExtensionFromFilePath(fileName);

	if (fileExtensions == ".dds") {
		ThrowIfFailed(LoadFromDDSFile(wideFileName.c_str(), DDS_FLAGS_FORCE_RGB, &metadata, scratchImage));
	}
	else if (fileExtensions == ".hdr") {
		ThrowIfFailed(LoadFromHDRFile(wideFileName.c_str(), &metadata, scratchImage));
	}
	else if (fileExtensions == ".tga") {
		ThrowIfFailed(LoadFromTGAFile(wideFileName.c_str(), &metadata, scratchImage));
	}
	else {
		ThrowIfFailed(LoadFromWICFile(wideFileName.c_str(), WIC_FLAGS_FORCE_RGB, &metadata, scratchImage));
	}

	if (sRBG) {
		metadata.format = MakeSRGB(metadata.format);
	}

	const auto d3d12Resource = impl::CreateD3D12Texture(metadata);

	auto texture = std::make_unique<Texture>(d3d12Resource);
	texture->SetName(wideFileName);

	//Update the global state tracker
	ResourceStateTracker::AddGlobalResourceState(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COMMON);

	const auto subResources = impl::CreateSubResources(scratchImage);
	ctx.CopyTextureSubresource(*texture, 0, subResources);

	if (subResources.size() < d3d12Resource->GetDesc().MipLevels) {
		ComputeMipsPass computeMipsPass(ctx);
		computeMipsPass.Execute();
	}

	impl::textureCache[wideFileName] = d3d12Resource.Get();

	return std::move(texture);
}
