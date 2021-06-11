#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>
#include <wrl.h>

#include "Utils/d3dx12.h"
#include <set>

namespace Crystal {
    class DescriptorAllocatorPage;
    class DescriptorAllocation {
    public:
        DescriptorAllocation();
        DescriptorAllocation(
            D3D12_CPU_DESCRIPTOR_HANDLE descriptor,
            uint32_t numHandles,
            uint32_t descriptorSize,
            std::shared_ptr<DescriptorAllocatorPage> page);

        DescriptorAllocation(const DescriptorAllocation&)            = delete;
        DescriptorAllocation& operator=(const DescriptorAllocation&) = delete;
        DescriptorAllocation(DescriptorAllocation&& allocation) noexcept;
        DescriptorAllocation& operator=(DescriptorAllocation&& other) noexcept;

        ~DescriptorAllocation();

        [[nodiscard]] bool IsNull() const noexcept;
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(uint32_t offset = 0) const;
        [[nodiscard]] uint32_t GetNumHandles() const noexcept { return m_numHandles; };
    private:
        void Free() noexcept;

        D3D12_CPU_DESCRIPTOR_HANDLE m_descriptor;
        uint32_t m_numHandles;
        uint32_t m_descriptorSize;
        std::shared_ptr<DescriptorAllocatorPage> m_page;
    };

    class DescriptorAllocatorPage final : public std::enable_shared_from_this<DescriptorAllocatorPage>{
    public:
        DescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);
        ~DescriptorAllocatorPage() = default;

        [[nodiscard]] bool HasSpace(uint32_t numDescriptors) const noexcept;
        [[nodiscard]] uint32_t NumFreeHandles() const noexcept { return m_numFreeHandles; }
        [[nodiscard]] DescriptorAllocation Allocate(uint32_t numDescriptors) noexcept;
        [[nodiscard]] uint32_t ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle) noexcept;

        void Free(DescriptorAllocation&& descriptor) noexcept;
        void ReleaseStaleDescriptors() noexcept;
        void AddNewBlock(uint32_t offset, uint32_t numDescriptors) noexcept;
        void FreeBlock(uint32_t offset, uint32_t numDescriptors) noexcept;
    private:
        struct FreeBlockInfo;
        using FreeListByOffset = std::map<size_t, FreeBlockInfo>;

        // A map that lists the free blocks by size.
        // Needs to be a multimap since multiple blocks can have the same size.
        using FreeListBySize = std::multimap<size_t, FreeListByOffset::iterator>;

        struct FreeBlockInfo {
            FreeBlockInfo(size_t size)
                :
                Size(size)
            {}
            size_t Size;
            FreeListBySize::iterator FreeListBySizeIt;
        };

        struct StaleDescriptorInfo {
            StaleDescriptorInfo(size_t offset, size_t size)
                :
                Offset(offset),
                Size(size)
            {}

            // The offset within the descriptor heap.
            size_t Offset;
            // The number of descriptors
            size_t Size;
        };

        using StaleDescriptorQueue = std::queue<StaleDescriptorInfo>;

        FreeListByOffset m_freeListByOffset;
        FreeListBySize m_freeListBySize;
        StaleDescriptorQueue m_staleDescriptors;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_d3d12DescriptorHeap;
        D3D12_DESCRIPTOR_HEAP_TYPE m_heapType;

        CD3DX12_CPU_DESCRIPTOR_HANDLE m_baseDescriptor;

        uint32_t m_descriptorHandleIncrementSize;
        uint32_t m_numDescriptorsInHeap;
        uint32_t m_numFreeHandles;

        std::mutex m_allocationMutex;
    };

    class DescriptorAllocator {
    public:
        DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors = 256) noexcept;
        ~DescriptorAllocator() = default;

        [[nodiscard]] DescriptorAllocation Allocate(uint32_t numDescriptors = 1) noexcept;
        void ReleaseStaleDescriptors() noexcept;
    private:
        friend struct std::default_delete<DescriptorAllocator>;
        [[nodiscard]] std::shared_ptr<DescriptorAllocatorPage> CreateAllocatorPage() noexcept;

        D3D12_DESCRIPTOR_HEAP_TYPE m_heapType;
        uint32_t m_numDescriptorsPerHeap;

        std::vector<std::shared_ptr<DescriptorAllocatorPage>> m_heapPool;

        //Indices of available heaps in the heap pool
        std::set<size_t> m_availableHeaps;
        std::mutex m_allocationMutex;
    };
}


