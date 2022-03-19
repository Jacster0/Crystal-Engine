#include "D3D12DynamicDescriptorHeap.h"
#include "D3D12Core.h"
#include "D3D12CommandContext.h"
#include "D3D12RootSignature.h"

#include "Utils/D3D12Exception.h"
#include "Core/Logging/Logger.h"
#include <stdexcept>

using namespace Crystal;
using namespace Microsoft::WRL;

DynamicDescriptorHeap::DynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptorsPerHeap) noexcept
    :
    m_descriptorHeapType(heapType),
    m_numDescriptorsPerHeap(numDescriptorsPerHeap),
    m_descriptorTableBitMask(0),
    m_currentCPUDescriptorHandle(D3D12_DEFAULT),
    m_currentGPUDescriptorHandle(D3D12_DEFAULT),
    m_numFreeHandles(0)
{
    auto& device = RHICore::get_device();

    m_descriptorHandleIncrementSize = device.GetDescriptorHandleIncrementSize(heapType);
    m_descriptorHandleCache         = std::make_unique<D3D12_CPU_DESCRIPTOR_HANDLE[]>(m_numDescriptorsPerHeap);
}

void Crystal::DynamicDescriptorHeap::StageDescriptors(
    const uint32_t rootParameterIndex,
    const uint32_t offset,
    const uint32_t numDescriptors,
    const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptors)
{
    // Cannot stage more descriptors than m_numDescriptorsPerHeap.
    // Cannot stage more than MAX_DESCRIPTOR_TABLES root parameters.
    if (numDescriptors > m_numDescriptorsPerHeap || rootParameterIndex >= MAX_DESCRIPTOR_TABLES) {
        crylog_error(
                "Maximun descriptors per heap is {}. Number of descriptors you are trying to stage: {}"
                "Maximun number of descriptor tables per root signature: {}. Current rootparameter index: {}",
                m_numDescriptorsPerHeap, numDescriptors, MAX_DESCRIPTOR_TABLES, rootParameterIndex);

        throw std::bad_alloc();
    }

    DescriptorTableCache& descriptorTableCache = m_descriptorTableCache[rootParameterIndex];
    
    // Check that the number of descriptors to copy does not exceed the number
    // of descriptors expected in the descriptor table.
    if ((offset + numDescriptors) > descriptorTableCache.NumDescriptors) {
        throw std::length_error("Number of descriptors exceeds the number of descriptors in the descriptor table.");
    }

    D3D12_CPU_DESCRIPTOR_HANDLE* const dstDescriptor = (descriptorTableCache.BaseDescriptor + offset);
    for (uint32_t i = 0; i < numDescriptors; i++) {
        dstDescriptor[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(srcDescriptors, i, m_descriptorHandleIncrementSize);
    }

    // Set the root parameter index bit to make sure the descriptor table 
    // at that index is bound to the command list.
    m_staleDescriptorTableBitMask |= (1 << rootParameterIndex);
}

void Crystal::DynamicDescriptorHeap::BindDescriptorsForDraw(CommandContext& context) noexcept {
    Bind(context, &ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable);
}

void DynamicDescriptorHeap::BindDescriptorsForDispatch(CommandContext& context) noexcept {
    Bind(context, &ID3D12GraphicsCommandList::SetComputeRootDescriptorTable);
}

D3D12_GPU_DESCRIPTOR_HANDLE DynamicDescriptorHeap::CopyDescriptor(
    CommandContext& commandList, 
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor) noexcept
{
    if (!m_currentDescriptorHeap || m_numFreeHandles < 1) {
        m_currentDescriptorHeap = RequestDescriptorHeap();
        m_currentCPUDescriptorHandle = m_currentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        m_currentGPUDescriptorHandle = m_currentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
        m_numFreeHandles = m_numDescriptorsPerHeap;

        commandList.SetDescriptorHeap(m_descriptorHeapType, m_currentDescriptorHeap.Get());

        m_staleDescriptorTableBitMask = m_descriptorTableBitMask;
    }

    auto& device = RHICore::get_device();

    D3D12_GPU_DESCRIPTOR_HANDLE gpuLocation = m_currentGPUDescriptorHandle;
    device.CopyDescriptorsSimple(1, m_currentCPUDescriptorHandle, cpuDescriptor, m_descriptorHeapType);

    m_currentCPUDescriptorHandle.Offset(1, m_descriptorHandleIncrementSize);
    m_currentGPUDescriptorHandle.Offset(1, m_descriptorHandleIncrementSize);
    m_numFreeHandles--;

    return gpuLocation;
}

void DynamicDescriptorHeap::StageInlineCBV(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferAddr) noexcept {
    m_inlineCBV[rootParameterIndex] = bufferAddr;
    m_staleCBVBitMask |= (1 << rootParameterIndex);
}

void DynamicDescriptorHeap::StageInlineSRV(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferAddr) noexcept {
    m_inlineSRV[rootParameterIndex] = bufferAddr;
    m_staleSRVBitMask |= (1 << rootParameterIndex);
}

void DynamicDescriptorHeap::StageInlineUAV(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferAddr) noexcept {
    m_inlineUAV[rootParameterIndex] = bufferAddr;
    m_staleUAVBitMask |= (1 << rootParameterIndex);
}

void DynamicDescriptorHeap::ParseRootSignature(const RootSignature* const rootSignature) noexcept {
    m_staleDescriptorTableBitMask = 0;
    const auto& rootSignatureDesc = rootSignature->GetRootSignatureDesc();

    // Get a bit mask that represents the root parameter indices that match the 
    // descriptor heap type for this dynamic descriptor heap.
    m_descriptorTableBitMask = rootSignature->GetDescriptorTableBitMask(m_descriptorHeapType);
    uint32_t descriptorTableBitMask = m_descriptorTableBitMask;

    uint32_t currentoffset = 0;

    constexpr auto numBits = sizeof(descriptorTableBitMask) * 8;

    //Only enter the loop if any of the bits are set
    if (descriptorTableBitMask > 0) [[likely]] {
       for (int i = 0; i < numBits; i++) {
           //If no bits are set, break the loop
           if (descriptorTableBitMask == 0) {
               break;
           }
           else if (((descriptorTableBitMask >> i) & 1) && (i < rootSignatureDesc.NumParameters)) {
               const uint32_t numDescriptors = rootSignature->GetNumDescriptors(i);

               DescriptorTableCache& descriptorTableCache = m_descriptorTableCache[i];
               descriptorTableCache.NumDescriptors        = numDescriptors;
               descriptorTableCache.BaseDescriptor        = m_descriptorHandleCache.get() + currentoffset;

               currentoffset += numDescriptors;

               //Flip the bit in order to reduce the number of iterations
               descriptorTableBitMask ^= (1 << i);
           }
       }
    }

    if (currentoffset <= m_numDescriptorsPerHeap) {
        crylog_warning("The root signature requires more than the maximum number of descriptors per descriptor heap."
            " Consider increasing the maximum number of descriptors per descriptor heap.");
    }
}

void Crystal::DynamicDescriptorHeap::Reset() noexcept {
    m_availableDescriptorHeaps    = m_descriptorHeapPool;
    m_currentDescriptorHeap.Reset();
    m_currentCPUDescriptorHandle  = CD3DX12_CPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
    m_currentGPUDescriptorHandle  = CD3DX12_GPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
    m_numFreeHandles              = 0;
    m_descriptorTableBitMask      = 0;
    m_staleDescriptorTableBitMask = 0;

    for (uint32_t i = 0; i < MAX_DESCRIPTOR_TABLES; i++) {
        m_descriptorTableCache[i].Reset();
    }
}

void DynamicDescriptorHeap::Bind(
    CommandContext& context,
    SetDescriptorTableCB setDescriptorTable) noexcept
{
    const auto numDescriptorsToBind = GetStaleDescriptorCount();

    if (numDescriptorsToBind > 0) {
        auto& device     = RHICore::get_device();
        auto d3d12commandList = context.GetNativeCommandList().Get();

        if (m_currentDescriptorHeap == nullptr || m_numFreeHandles < numDescriptorsToBind) {
            m_currentDescriptorHeap      = RequestDescriptorHeap();
            m_currentCPUDescriptorHandle = m_currentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            m_currentGPUDescriptorHandle = m_currentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
            m_numFreeHandles             = m_numDescriptorsPerHeap;

            context.SetDescriptorHeap(m_descriptorHeapType, m_currentDescriptorHeap.Get());

            m_staleDescriptorTableBitMask = m_descriptorTableBitMask;
        }

        static constexpr auto numBits = sizeof(m_staleDescriptorTableBitMask) * 8;

        //Only enter the loop if any of the bits are set
        if (m_staleDescriptorTableBitMask > 0) [[likely]] {
            for (int i = 0; i < numBits; i++) {
                //if no bits are set we exit the loop
                if (m_staleDescriptorTableBitMask == 0) {
                    break;
                }
                else if ((m_staleDescriptorTableBitMask >> i) & 1) {
                    uint32_t numDescriptors                            = m_descriptorTableCache[i].NumDescriptors;
                    D3D12_CPU_DESCRIPTOR_HANDLE* pSrcDescriptorHandles = m_descriptorTableCache[i].BaseDescriptor;

                    std::array destDescriptorRangeStarts = { m_currentCPUDescriptorHandle };
                    std::array destDescriptorRangeSizes  = { numDescriptors };

                    device.CopyDescriptors(
                        1,
                        destDescriptorRangeStarts.data(),
                        destDescriptorRangeSizes.data(),
                        numDescriptors,
                        pSrcDescriptorHandles,
                        nullptr,
                        m_descriptorHeapType);

                    setDescriptorTable(d3d12commandList, i, m_currentGPUDescriptorHandle);

                    m_currentCPUDescriptorHandle.Offset(numDescriptors, m_descriptorHandleIncrementSize);
                    m_currentGPUDescriptorHandle.Offset(numDescriptors, m_descriptorHandleIncrementSize);

                    m_numFreeHandles -= numDescriptors;

                    //Flip the bit in order to reduce the number of iterations.
                    m_staleDescriptorTableBitMask ^= (1 << i);
                }
            }
        }
    }
}

ComPtr<ID3D12DescriptorHeap> DynamicDescriptorHeap::RequestDescriptorHeap() {
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;

    if (!m_availableDescriptorHeaps.empty()) {
        descriptorHeap = m_availableDescriptorHeaps.front();
        m_availableDescriptorHeaps.pop();
    }
    else {
        descriptorHeap = CreateDescriptorHeap();
        m_descriptorHeapPool.push(descriptorHeap);
    }

    return descriptorHeap;
}

ComPtr<ID3D12DescriptorHeap> DynamicDescriptorHeap::CreateDescriptorHeap() {
    auto& device = RHICore::get_device();

    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesk = {
        .Type           = m_descriptorHeapType,
        .NumDescriptors = m_numDescriptorsPerHeap,
        .Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
    };

    ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    ThrowIfFailed(device.CreateDescriptorHeap(&descriptorHeapDesk, IID_PPV_ARGS(&descriptorHeap)));

    return descriptorHeap;
}

constexpr uint32_t DynamicDescriptorHeap::GetStaleDescriptorCount() const noexcept {
    uint32_t numStaleDescriptors = 0;
    uint32_t staleDescriptorBitMask = m_staleDescriptorTableBitMask;

    constexpr auto numBits = sizeof(staleDescriptorBitMask) * 8;

    //only enter the loop if any of the bits are set 
    if (staleDescriptorBitMask > 0) [[likely]] {
        for (uint32_t i = 0; i < numBits; i++) {
            //if no bits are set, break the loop.
            if (staleDescriptorBitMask == 0) {
                break;
            }
            else if ((staleDescriptorBitMask >> i) & 1) {
                numStaleDescriptors += m_descriptorTableCache[i].NumDescriptors;

                //Flip the bit in order to reduce the number of iterations
                staleDescriptorBitMask ^= (1 << i);
            }
        }
    }

    return numStaleDescriptors;
}

