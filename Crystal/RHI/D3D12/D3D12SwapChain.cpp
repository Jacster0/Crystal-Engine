#include "D3D12SwapChain.h"
#include "D3D12Core.h"
#include "D3D12CommandQueue.h"
#include "D3D12CommandContext.h"
#include "D3D12RenderTarget.h"
#include "D3D12Texture.h"
#include "Utils/D3D12Exception.h"
#include "Utils/ResourceStateTracker.h"
#include "Core/Math/Rectangle.h"

#include <cassert>
#include <algorithm>
#include <chrono>

#include "Core/Logging/Logger.h"

using namespace Crystal;
using namespace Microsoft::WRL;
using namespace std::chrono_literals;

SwapChain::SwapChain(HWND hWnd, DXGI_FORMAT renderTargetFormat)
	:
	m_hWnd(hWnd),
	m_renderTargetFormat(renderTargetFormat)
{
	assert(hWnd);

	auto& d3d12CommandQueue = RHICore::get_graphics_queue();
	auto& adapter           = RHICore::get_physical_device();

	ComPtr<IDXGIFactory>  dxgiFactory;
	ComPtr<IDXGIFactory5> dxgiFactory5;

	ThrowIfFailed(adapter.GetParent(IID_PPV_ARGS(&dxgiFactory)));
	ThrowIfFailed(dxgiFactory.As(&dxgiFactory5));

	bool allowTearing = false;
	if (SUCCEEDED(
		dxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(bool))))
	{
		m_tearingSupported = allowTearing == true;
	}

	Math::Rectangle windowRect;
	GetClientRect(hWnd, reinterpret_cast<RECT*>(&windowRect));

	m_width  = windowRect.Width();
	m_height = windowRect.Height();

	uint32_t flags = (m_tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);
	flags         |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{
		.Width       = m_width,
		.Height      = m_height,
		.Format      = m_renderTargetFormat,
		.Stereo      = false,
		.SampleDesc  = {1,0},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = BUFFER_COUNT,
		.Scaling     = DXGI_SCALING_STRETCH,
		.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.AlphaMode   = DXGI_ALPHA_MODE_UNSPECIFIED,
		.Flags       = flags,
	};

	ComPtr<IDXGISwapChain1> swapChain;
	ThrowIfFailed(dxgiFactory5->CreateSwapChainForHwnd(
		d3d12CommandQueue.GetNativeCommandQueue().Get(),
		m_hWnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain));

	ThrowIfFailed(swapChain.As(&m_dxgiSwapChain));
	ThrowIfFailed(dxgiFactory5->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));

	m_currentBackbufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();
	m_dxgiSwapChain->SetMaximumFrameLatency(BUFFER_COUNT - 1);
	m_frameLatencyWaitableObject = m_dxgiSwapChain->GetFrameLatencyWaitableObject();

	UpdateRenderTargetViews();
}

uint32_t SwapChain::Present(const Texture* const texture) {
	auto& queue           = RHICore::get_graphics_queue();
	auto& ctx             = queue.GetCommandContext();
	const auto backBuffer = m_backbufferTextures.at(m_currentBackbufferIndex).get();

	if (texture) [[likely]] {
		if (texture->GetResourceDesc().SampleDesc.Count > 1) {
			ctx.ResolveSubResource(backBuffer, texture);
		}
		else {
			ctx.CopyResource(*backBuffer, *texture);
		}
	}

	ctx.TransitionResource(backBuffer, { { ResourceState_t::present } });
	queue.Submit(&ctx);

	const auto syncInterval = static_cast<uint32_t>(m_vSync);
	const uint32_t presentFlags = (m_tearingSupported && !m_fullscreen && m_vSync == VSync::Off) ? DXGI_PRESENT_ALLOW_TEARING : 0u;

	ThrowIfFailed(m_dxgiSwapChain->Present(syncInterval, presentFlags));

	m_fenceValues[m_currentBackbufferIndex] = queue.Signal();
	m_currentBackbufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

	RHICore::release_stale_descriptors();

	return m_currentBackbufferIndex;
}

void Crystal::SwapChain::Resize(USize size) {
	Resize(size.Width, size.Height);
}

void SwapChain::Resize(uint32_t width, uint32_t height) {
	if (m_width != width || m_height != height) [[likely]] {
		m_width  = std::max(1u, width);
		m_height = std::max(1u, height);

		//Release all references to back buffer textures.
		m_renderTarget.Reset();

		for (uint32_t i = 0; i < BUFFER_COUNT; i++) {
			m_backbufferTextures[i].reset();
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		ThrowIfFailed(m_dxgiSwapChain->GetDesc(&swapChainDesc));
		ThrowIfFailed(m_dxgiSwapChain->ResizeBuffers(
			BUFFER_COUNT,
			m_width,
			m_height,
			swapChainDesc.BufferDesc.Format,
			swapChainDesc.Flags));

		m_currentBackbufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();
		UpdateRenderTargetViews();
	}
}

void SwapChain::WaitForSwapChain() noexcept {
	constexpr auto duration = 1000ms;
	auto result = WaitForSingleObjectEx(m_frameLatencyWaitableObject, duration.count(), true);
}

const RenderTarget& SwapChain::GetRenderTarget() const noexcept {
	m_renderTarget.AttachTexture(AttachmentPoint::Color0, m_backbufferTextures[m_currentBackbufferIndex]);

	return m_renderTarget;
}

RenderTarget& SwapChain::GetRenderTarget() noexcept {
	m_renderTarget.AttachTexture(AttachmentPoint::Color0, m_backbufferTextures[m_currentBackbufferIndex]);

	return m_renderTarget;
}

void SwapChain::UpdateRenderTargetViews() {
	for (uint32_t i = 0; i < BUFFER_COUNT; i++) {
		ComPtr<ID3D12Resource> backBuffer;
		ThrowIfFailed(m_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

		ResourceStateTracker::AddGlobalResourceState(backBuffer.Get(), D3D12_RESOURCE_STATE_COMMON);

		m_backbufferTextures[i] = std::make_shared<Texture>(backBuffer);
		m_backbufferTextures[i]->SetName(L"Backbuffer[" + std::to_wstring(i) + L"]");
	}
}
