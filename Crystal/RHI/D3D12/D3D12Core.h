#pragma once
#include "D3D12DescriptorHeap.h"
#include "Utils/d3dx12.h"
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <memory>
#include <array>
#include "Graphics/Types/Types.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

namespace Crystal { class CommandQueue; }

namespace Crystal::RHICore {
	using Crystal::CommandQueue;

	void initialize();

	[[nodiscard]] ID3D12Device8& get_device() noexcept;
	[[nodiscard]] IDXGIAdapter4& get_physical_device() noexcept;
	[[nodiscard]] std::wstring get_physical_device_description() noexcept;
	[[nodiscard]] D3D_ROOT_SIGNATURE_VERSION get_highest_root_signature_version() noexcept;

	[[nodiscard]] DescriptorAllocation allocate_descriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors = 1) noexcept;
	void release_stale_descriptors() noexcept;

	[[nodiscard]] CommandQueue& get_graphics_queue() noexcept;
	[[nodiscard]] CommandQueue& get_compute_queue() noexcept;
	[[nodiscard]] CommandQueue& get_copy_queue()    noexcept;
}