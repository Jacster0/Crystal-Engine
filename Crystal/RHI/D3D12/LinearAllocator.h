#pragma once
#include "../Core/Memory/MemoryConstants.h"
#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include <deque>

namespace Crystal {
	struct Allocation {
		void* CPU;
		D3D12_GPU_VIRTUAL_ADDRESS GPU;
	};

	class LinearAllocatorPage {
	public:
		explicit LinearAllocatorPage(size_t sizeInBytes);
		LinearAllocatorPage(const LinearAllocatorPage& rhs)            = delete;
		LinearAllocatorPage& operator=(const LinearAllocatorPage& rhs) = delete;
		~LinearAllocatorPage();

		[[nodiscard]] bool HasSpace(size_t sizeInBytes, size_t alignment) const noexcept;
		[[nodiscard]] Allocation Allocate(const size_t sizeInBytes, const size_t alignment);
		void Reset() noexcept;

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource{nullptr};
		void* m_cpuPtr{ nullptr };
		D3D12_GPU_VIRTUAL_ADDRESS m_gpuPtr{ D3D12_GPU_VIRTUAL_ADDRESS(0)};

		size_t m_pageSize{};
		size_t m_offset{ 0 };
	};

	class LinearAllocator {
	public:
		explicit LinearAllocator(size_t pageSize = Crystal::_2MB) noexcept;
		~LinearAllocator();

		[[nodiscard]] constexpr size_t GetPageSize() const noexcept { return m_pageSize; }
		[[nodiscard]] Allocation Allocate(const size_t sizeInBytes, const size_t alignment);
		void Reset() noexcept;
	private:
		[[nodiscard]] std::shared_ptr<LinearAllocatorPage> RequestPage() noexcept;

		std::deque<std::shared_ptr<LinearAllocatorPage>> m_pagePool;
		std::deque<std::shared_ptr<LinearAllocatorPage>> m_availablePages;

		std::shared_ptr<LinearAllocatorPage> m_currentPage;
		size_t m_pageSize;
	};
}