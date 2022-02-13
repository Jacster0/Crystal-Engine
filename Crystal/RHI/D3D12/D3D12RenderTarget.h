#pragma once
#include <vector>
#include <memory>
#include "Core/Lib/CrystalTypes.h"
#include "Graphics/Viewport.h"
#include "Platform/Windows/CrystalWindow.h"
#include <d3d12.h>

namespace Crystal {
    enum AttachmentPoint {
        Color0,
        Color1,
        Color2,
        Color3,
        Color4,
        Color5,
        Color6,
        Color7,
        DepthStencil,
        NumAttachmentPoints,
    };
   
    class Texture;
    class RenderTarget {
    public:
        void AttachTexture(AttachmentPoint attachmentPoint, std::shared_ptr<Texture> texture) noexcept;
        [[nodiscard]] std::shared_ptr<Texture> GetTexture(AttachmentPoint attachmentPoint) const noexcept;

        void Resize(USize size) noexcept;
        void Resize(uint32_t width, uint32_t height) noexcept;

        [[nodiscard]] constexpr USize GetSize() const noexcept { return m_size; }
        [[nodiscard]] constexpr uint32_t GetWidth() const noexcept { return m_size.Width; }
        [[nodiscard]] constexpr uint32_t GetHeight() const noexcept { return m_size.Height; }

        [[nodiscard]] Viewport GetViewPort(
            FSize scale    = { 1.0f, 1.0f }, 
            FSize bias     = { 0.0f, 0.0f }, 
            float minDepth = 0.0f, 
            float maxDepth = 1.0f) const noexcept;

        [[nodiscard]] const std::vector<std::shared_ptr<Texture>>& GetTextures() const noexcept;
        [[nodiscard]] D3D12_RT_FORMAT_ARRAY GetRenderTargetFormats() const noexcept;
        [[nodiscard]] DXGI_FORMAT GetDepthStencilFormat() const noexcept;
        [[nodiscard]] DXGI_SAMPLE_DESC GetSampleDesc() const noexcept;

        void Reset() { m_textures = std::vector<std::shared_ptr<Texture>>(AttachmentPoint::NumAttachmentPoints); }
    private:
        std::vector<std::shared_ptr<Texture>> m_textures{ AttachmentPoint::NumAttachmentPoints };
        USize m_size{ 0,0 };
    };
}