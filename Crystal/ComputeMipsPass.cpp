#include "ComputeMipsPass.h"
#include "RHI/D3D12/Utils/d3dx12.h"
#include <array>
#include "RHI/D3D12/D3D12RootSignature.h"
#include "RHI/D3D12/D3D12PipelineState.h"
#include "RHI/D3D12/D3D12Core.h"
#include "RHI/D3D12/D3D12CommandContext.h"
#include <ranges>

#include "RHI/D3D12/D3D12Texture.h"
#include "RHI/D3D12/Utils/D3D12Exception.h"
#include "RHI/D3D12/Utils/ResourceStateTracker.h"

using namespace Crystal;
using namespace Microsoft::WRL;

ComputeMipsPass::ComputeMipsPass(CommandContext& ctx)
    :
    ComputePass(ctx)
{
    ComputeMipsPass::Prepare();
}

void ComputeMipsPass::Prepare() {
	if (!m_texture) {
		return;
	}

	auto& computeCtx = m_ctx.AsComputeContext();

	CreateRootSignature();
	CreatePSO();
	CreateUAVS();

	auto& device = RHICore::get_device();
	const auto resource = m_texture->GetUnderlyingResource();

	if (!resource) {
		return;
	}

	auto resourceDesc = resource->GetDesc();

	//If the texture only has a single mip, do nothing
	if (m_texture->MipmapCount() == 1) {
		return;
	}

	if (m_texture->GetTextureType() != TextureType::Texture2D) {
		if (resourceDesc.DepthOrArraySize != 1 || m_texture->IsMultiSampled()) {
			throw std::exception("Computing mipmaps only supported for non-multi-sampled 2D Textures");
		}
	}

	ComPtr<ID3D12Resource> uavResource = resource;
	ComPtr<ID3D12Resource> aliasResource;

	if (!m_texture->CheckUAVSupport() || (resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) == 0) {
		auto aliasDesc = resourceDesc;

		aliasDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		aliasDesc.Flags &= ~(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		auto uavDesc = aliasDesc;
		uavDesc.Format = Texture::GetUAVCompatableFormat(resourceDesc.Format);

		const std::array resourceDescs{ aliasDesc, uavDesc };

		const auto [SizeInBytes, Alignment] = device.GetResourceAllocationInfo(0, resourceDescs.size(), resourceDescs.data());

		const D3D12_HEAP_DESC heapDesc{
			.SizeInBytes = SizeInBytes,
			.Properties {
				.Type = D3D12_HEAP_TYPE_DEFAULT,
				.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
				.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
			},
			.Alignment = Alignment,
			.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES,
		};

		ComPtr<ID3D12Heap> heap;
		ThrowIfFailed(device.CreateHeap(&heapDesc, IID_PPV_ARGS(&heap)));

		computeCtx.TrackResource(heap);

		ThrowIfFailed(device.CreatePlacedResource(
			heap.Get(),
			0,
			&aliasDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&aliasResource)));

		ResourceStateTracker::AddGlobalResourceState(aliasResource.Get(), D3D12_RESOURCE_STATE_COMMON);
		computeCtx.TrackResource(aliasResource);

		ThrowIfFailed(device.CreatePlacedResource(
			heap.Get(),
			0,
			&uavDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&uavResource)));

		ResourceStateTracker::AddGlobalResourceState(uavResource.Get(), D3D12_RESOURCE_STATE_COMMON);
		computeCtx.TrackResource(uavResource);

		computeCtx.InsertAliasingBarrier({ nullptr }, aliasResource);
		computeCtx.CopyResource(aliasResource, resource);
		computeCtx.InsertAliasingBarrier(aliasResource, uavResource);
	}
}

void ComputeMipsPass::Execute() {
	bool isSrgb = false;
	GenerateMipsCB generateMipsCB;
	auto& computeCtx = m_ctx.AsComputeContext();

	computeCtx.SetPipelineState(&GetPipelineState());
	computeCtx.SetComputeRootSignature(&GetRootSignature());

	const auto resourceDesc = m_texture->GetResourceDesc();

	DWORD mipCount{};
	for (uint32_t srcMip = 0; srcMip < resourceDesc.MipLevels; srcMip += mipCount) {
		const auto srcWidth = resourceDesc.Width >> srcMip;
		const auto srcHeight = resourceDesc.Height >> srcMip;
		auto dstWidth = srcWidth >> 1;
		auto dstHeight = srcHeight >> 1;

		// 0b00(0): Both width and height are even
		// 0b01(1): Width is odd, height is even
		// 0b10(2): Width is even, height is odd
		// 0b11(3): Both width and height are odd
		generateMipsCB.SrcDimension = (srcHeight & 1) << 1 | (srcWidth & 1);

		_BitScanForward(&mipCount, (dstWidth == 1 ? dstHeight : dstWidth | dstHeight == 1 ? dstWidth : dstHeight));

		mipCount = std::min<DWORD>(4, mipCount + 1);
		mipCount = (srcMip + mipCount >= resourceDesc.MipLevels) ? resourceDesc.MipLevels - srcMip - 1 : mipCount;

		//Restrict the dimensions from becoming 0
		dstWidth  = std::max<DWORD>(1, dstWidth);
		dstHeight = std::max<DWORD>(1, dstHeight);

		generateMipsCB.SrcMipLevel = srcMip;
		generateMipsCB.NumMipLevels = mipCount;
		generateMipsCB.TexelSize.x = 1.0f / static_cast<float>(dstWidth);
		generateMipsCB.TexelSize.y = 1.0f / static_cast<float>(dstHeight);

		computeCtx.SetComputeShaderConstants(static_cast<uint32_t>(GenerateMips::GenerateMipsCB), 4, &generateMipsCB);
		computeCtx.AsGraphicsContext().SetShaderResourceView(
			static_cast<uint32_t>(GenerateMips::SrcMip), 
			0, 
			*m_texture, 
			ResourceState_t::non_pixel_shader_resource, 
			srcMip, 
			1);

		for (uint32_t mip = 0; mip < mipCount; mip++) {
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{
				.Format = resourceDesc.Format,
				.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D,
				.Texture2D {
					.MipSlice = srcMip + mip + 1
				}
			};

			computeCtx.AsGraphicsContext().SetUnorderedAccessView(
				static_cast<uint32_t>(GenerateMips::OutMip), 
				mip, 
				*m_texture, ResourceState_t::unordered_access, 
				srcMip + mip + 1, 
				1);
		}

		computeCtx.Dispatch(Math::DivideByMultiple(1, 8u), Math::DivideByMultiple(10, 8u));
		computeCtx.InsertUAVBarrier(*m_texture);
	}
}

void ComputeMipsPass::CreateRootSignature() noexcept {
    const CD3DX12_DESCRIPTOR_RANGE1 srcMip(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
    const CD3DX12_DESCRIPTOR_RANGE1 outMip(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 4, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    std::array<CD3DX12_ROOT_PARAMETER1, 4> rootParameters{};

    rootParameters[0].InitAsConstants(sizeof(GenerateMipsCB) / 4, 0);
    rootParameters[1].InitAsDescriptorTable(1, &srcMip);
    rootParameters[2].InitAsDescriptorTable(1, &outMip);

    const CD3DX12_STATIC_SAMPLER_DESC linearClampSampler(
        0,
        D3D12_FILTER_MIN_MAG_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

    const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc(rootParameters.size(), rootParameters.data(), 1, &linearClampSampler);

    m_rootSignature = std::make_unique<RootSignature>();
    m_rootSignature->Intialize(rootSignatureDesc.Desc_1_1);
}

void ComputeMipsPass::CreatePSO() noexcept {
    ComPtr<ID3DBlob> generateMipsBlob;

    struct PipelineStateStream {
        CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
        CD3DX12_PIPELINE_STATE_STREAM_CS Cs;
    } pipelineStateStream {
        .RootSignature = m_rootSignature->GetRootSignature().Get(),
        .Cs = CD3DX12_SHADER_BYTECODE(generateMipsBlob.Get())
    };

    m_pipeLineState = std::make_unique<PipelineState>(PipelineStateSpecification {
        sizeof(pipelineStateStream),
        static_cast<void*>(&pipelineStateStream)
    });
}

void ComputeMipsPass::CreateUAVS() noexcept {
    m_defaultUAV = RHICore::allocate_descriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4);

    for(const auto i : std::views::iota(0,4)) {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc {};
        uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Texture2D.MipSlice = i;
        uavDesc.Texture2D.PlaneSlice = 0;

        RHICore::get_device().CreateUnorderedAccessView(nullptr, nullptr, &uavDesc, m_defaultUAV.GetDescriptorHandle(i));
    }
}
