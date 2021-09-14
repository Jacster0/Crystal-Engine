#pragma once
#include "../Platform/Windows/CrystalWindow.h"
#include "../RHI/RenderTarget.h"
#include "Viewport.h"
#include "Camera.h"

#include <cstdint>
#include <memory>

namespace Crystal {
    class SwapChain;
	class Graphics {
    public:
        Graphics()                               = default;
        Graphics(const Graphics& rhs)            = delete;
        Graphics& operator=(const Graphics& rhs) = delete;
        Graphics(Graphics&& rhs)                 = delete;
        Graphics& operator=(Graphics&& rhs)      = delete;
        ~Graphics()                              = default;

        void Initialize(uint32_t width, uint32_t height);
        void Resize(uint32_t width, uint32_t height);
        void SetWindowHandle(HWND hWnd) noexcept { m_hWnd = hWnd; }
    private:
        std::unique_ptr<SwapChain> m_swapChain;

        uint32_t m_clientWidth;
        uint32_t m_clientHeight;

        HWND m_hWnd;

        bool m_isIntialized{ false };
        Viewport m_viewPort;
        RenderTarget m_renderTarget;
        Camera m_camera;
	};
}