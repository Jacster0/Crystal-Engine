#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include "../Graphics/Types/Types.h"

namespace Crystal {
    struct BufferDescription {
        uint32_t Count{};
        uint32_t Stride{};
        IndexFormat_t Format = IndexFormat_t::unknown;
    };

    class Texture;
    class Buffer {
    public:
        explicit Buffer(const BufferDescription& desc);
        Buffer(const BufferDescription& desc, const Texture* const texture) noexcept;

        [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS BufferLocation() const noexcept { return m_resource->GetGPUVirtualAddress(); };
        [[nodiscard]] uint32_t Count()  const noexcept { return m_bufferDesc.Count; };
        [[nodiscard]] uint32_t Stride() const noexcept { return m_bufferDesc.Stride; };
        [[nodiscard]] uint32_t Size()   const noexcept { return m_bufferDesc.Count * m_bufferDesc.Stride; };
        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() const noexcept { return m_resource; };
        [[nodiscard]] const BufferDescription& GetDesc() const noexcept { return m_bufferDesc; };
    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
        BufferDescription m_bufferDesc;
    };
}
