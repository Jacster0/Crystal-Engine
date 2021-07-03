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

namespace Crystal {
	class RHICore {
	public:
		RHICore();
		static void Intialize();

		static [[nodiscard]] ID3D12Device8& GetDevice() noexcept { return *m_d3d12Device.Get(); }
		static [[nodiscard]] IDXGIAdapter4& GetPhysicalDevice() noexcept { return *m_dxgiAdapter.Get(); }
		static [[nodiscard]] std::wstring GetPhysicalDeviceDescription() noexcept { return m_adapterDesc.Description; }
		static [[nodiscard]] D3D_ROOT_SIGNATURE_VERSION GetHighestRootSignatureVersion() noexcept { return m_highestRootSignatureVersion; }

		static [[nodiscard]] DescriptorAllocation AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors = 1) noexcept;
		static void ReleaseStaleDescriptors() noexcept;
	private:
		static void CreatePhysicalDevice();
		static void CreateDevice();
		static void EnableDebugLayer();
		static void ReportLiveObjects() noexcept;

		static Microsoft::WRL::ComPtr<ID3D12Device8> m_d3d12Device;
		static Microsoft::WRL::ComPtr<IDXGIAdapter4> m_dxgiAdapter;

		static DXGI_ADAPTER_DESC3 m_adapterDesc;
		static D3D_ROOT_SIGNATURE_VERSION m_highestRootSignatureVersion;

		static std::array<std::unique_ptr<DescriptorAllocator>, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> m_descriptorAllocators;
		static bool m_isInitialized;
	};
}