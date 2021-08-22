#pragma once
#include "D3D12DescriptorHeap.h"
#include "Utils/d3dx12.h"
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <memory>
#include <array>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

namespace Crystal::RHICore {
	void Intialize();

	[[nodiscard]] inline ID3D12Device8& GetDevice() noexcept;
	[[nodiscard]] inline IDXGIAdapter4& GetPhysicalDevice() noexcept;
	[[nodiscard]] const inline std::wstring& GetPhysicalDeviceDescription() noexcept;
	[[nodiscard]] const inline D3D_ROOT_SIGNATURE_VERSION GetHighestRootSignatureVersion() noexcept;

	[[nodiscard]] inline DescriptorAllocation AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors = 1) noexcept;
	inline void ReleaseStaleDescriptors() noexcept;
}