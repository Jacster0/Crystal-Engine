#include "D3D12Core.h"
#include "Utils/D3D12Exception.h"
#include "D3D12CommandQueue.h"
#include "D3D12CommandContext.h"
#include "Graphics/Types/Types.h"

#include <dxgidebug.h>

using namespace Crystal;
using namespace Microsoft::WRL;

namespace impl {	
	static struct Data_t {
		Microsoft::WRL::ComPtr<ID3D12Device8> D3d12Device;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> DxgiAdapter;

		DXGI_ADAPTER_DESC3 AdapterDesc;
		D3D_ROOT_SIGNATURE_VERSION HighestRootSignatureVersion;

		std::array<std::unique_ptr<DescriptorAllocator>, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> DescriptorAllocators;

		std::unique_ptr<CommandQueue> GraphicsQueue;
		std::unique_ptr<CommandQueue> ComputeQueue;
		std::unique_ptr<CommandQueue> CopyQueue;

		bool IsInitialized;
	} data;

	void create_physical_device() {
		ComPtr<IDXGIFactory7> dxgiFactory7;
		ComPtr<IDXGIAdapter>  dxgiAdapter;
		ComPtr<IDXGIAdapter4> dxgiAdapter4;

		constexpr DXGI_GPU_PREFERENCE gpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
		uint32_t createFactoryFlags = 0;
		bool useWarp = true;

#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory7)));

		auto findBestAdapter = [&](uint32_t index) {
			return dxgiFactory7->EnumAdapterByGpuPreference(index, gpuPreference, IID_PPV_ARGS(&dxgiAdapter));
		};

		//Try to find the highest performant display subsystem 
		for (uint32_t i = 0; findBestAdapter(i) != DXGI_ERROR_NOT_FOUND; i++) {
			const auto hr = D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);

			if (SUCCEEDED(hr)) {
				ThrowIfFailed(dxgiAdapter.As(&dxgiAdapter4));

				//We found a display subsystem, so we don't need to fallback on a warp.
				useWarp = false;
				break;
			}
		}

		//Fallback on a software renderer if Direct3D hardware is not available 
		if (useWarp) [[unlikely]] {
			ThrowIfFailed(dxgiFactory7->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter)));
			ThrowIfFailed(dxgiAdapter.As(&dxgiAdapter4));
		}

			if (dxgiAdapter4) [[likely]] {
				ThrowIfFailed(dxgiAdapter4->GetDesc3(&data.AdapterDesc));
			}

		data.DxgiAdapter = dxgiAdapter4;
	}

	void create_device() {
		ThrowIfFailed(D3D12CreateDevice(data.DxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&data.D3d12Device)));

		ComPtr<ID3D12InfoQueue> infoQueue;

		if (SUCCEEDED(data.D3d12Device.As(&infoQueue))) {
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

			std::array severities = { D3D12_MESSAGE_SEVERITY_INFO };
			std::array denyIds = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
			};

			D3D12_INFO_QUEUE_FILTER newFilter = {
				.DenyList = {
					.NumSeverities = severities.size(),
					.pSeverityList = severities.data(),
					.NumIDs = denyIds.size(),
					.pIDList = denyIds.data()
				}
			};

			ThrowIfFailed(infoQueue->PushStorageFilter(&newFilter));
		}
	}

	void enable_debug_layer() {
		ComPtr<ID3D12Debug> debugInterface;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
		debugInterface->EnableDebugLayer();
	}

	void report_live_objects() noexcept {
		IDXGIDebug1* dxgiDebug;
		DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));

		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_IGNORE_INTERNAL);
		dxgiDebug->Release();
	}
}

using namespace impl;

void RHICore::initialize() {
	if (data.IsInitialized) {
		return;
	}

#ifdef _DEBUG
	enable_debug_layer();
#endif 

	create_physical_device();
	create_device();

	//Create command queues
	data.GraphicsQueue = std::make_unique<CommandQueue>(CommandListType_t::direct);
	data.ComputeQueue  = std::make_unique<CommandQueue>(CommandListType_t::compute);
	data.CopyQueue     = std::make_unique<CommandQueue>(CommandListType_t::copy);

	//Create descriptor allocators
	for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++) {
		data.DescriptorAllocators[i] = std::make_unique<DescriptorAllocator>(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
	}


	//Acquire highest root signature version
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{ .HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1 };

	if (FAILED(data.D3d12Device->CheckFeatureSupport(
		D3D12_FEATURE_ROOT_SIGNATURE, 
		&featureData, 
		sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE)))) 
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}
	data.HighestRootSignatureVersion = featureData.HighestVersion;

	data.IsInitialized = true;
}

ID3D12Device8& RHICore::get_device() noexcept { return *data.D3d12Device.Get(); }

IDXGIAdapter4& RHICore::get_physical_device() noexcept { return *data.DxgiAdapter.Get(); }

std::wstring RHICore::get_physical_device_description() noexcept { return { data.AdapterDesc.Description }; }

D3D_ROOT_SIGNATURE_VERSION RHICore::get_highest_root_signature_version() noexcept { return data.HighestRootSignatureVersion; }

DescriptorAllocation RHICore::allocate_descriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors) noexcept {
	return data.DescriptorAllocators[type]->Allocate(numDescriptors);
}

void RHICore::release_stale_descriptors() noexcept {
	for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++) {
		data.DescriptorAllocators[i]->ReleaseStaleDescriptors();
	}
}

CommandQueue& RHICore::get_graphics_queue() noexcept { return *data.GraphicsQueue.get(); }
CommandQueue& RHICore::get_compute_queue()  noexcept { return *data.ComputeQueue.get(); }
CommandQueue& RHICore::get_copy_queue()     noexcept { return *data.CopyQueue.get(); }
