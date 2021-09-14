#pragma once
#include "D3D12RenderTarget.h"
#include "../Graphics/Types/Types.h"

#include <dxgi1_5.h>     
#include <wrl/client.h>  
#include <array>

namespace Crystal {
	enum class VSync { On = true, Off = false };

	class Texture;
	class SwapChain {
	public:
		static constexpr uint32_t BUFFER_COUNT = 3;

		SwapChain(HWND hWnd, DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_R10G10B10A2_UNORM);
		uint32_t Present(const Texture* const texture = nullptr);
		void Resize(uint32_t width, uint32_t height);
		void WaitForSwapChain() noexcept;

		[[nodiscard]] Microsoft::WRL::ComPtr<IDXGISwapChain4> GetUnderlyingSwapChain() const noexcept { return m_dxgiSwapChain; }
		[[nodiscard]] const RenderTarget& GetRenderTarget() const noexcept;
		[[nodiscard]] RenderTarget& GetRenderTarget() noexcept;
		[[nodiscard]] DXGI_FORMAT GetRenderTargetFormat() const noexcept { return m_renderTargetFormat; }

		[[nodiscard]] bool TearingSupported() const noexcept { return m_tearingSupported; }

		void ToggleVsync() noexcept { SetVsync((m_vSync == VSync::On) ? VSync::Off : VSync::On); }
		void SetVsync(VSync vSync) noexcept { m_vSync = vSync; }
		[[nodiscard]] VSync GetVsync() const noexcept { return m_vSync; }

		void SetFullscreen(bool fullscreen) noexcept { m_fullscreen = fullscreen; }
		void ToggleFullScreen() noexcept { SetFullscreen(!m_fullscreen); }
		[[nodiscard]] bool IsFullScreen() const noexcept { return m_fullscreen; }
	private:
		void UpdateRenderTargetViews();

		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_dxgiSwapChain;

		std::array<std::shared_ptr<Texture>, BUFFER_COUNT> m_backbufferTextures;
		std::array<uint64_t, BUFFER_COUNT> m_fenceValues;

		mutable RenderTarget m_renderTarget;

		HANDLE m_frameLatencyWaitableObject;
		HWND m_hWnd;

		DXGI_FORMAT m_renderTargetFormat;

		uint32_t m_currentBackbufferIndex;
		uint32_t m_width;
		uint32_t m_height;

		VSync m_vSync{ VSync::Off };
		bool m_tearingSupported;
		bool m_fullscreen;
	};
}