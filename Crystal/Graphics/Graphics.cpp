#include "Graphics.h"

#include "../RHI/RHICore.h"
#include "../Core/Logging/Logger.h"
#include "../Core/Utils/StringUtils.h"

using namespace Crystal;

void Graphics::Initialize(uint32_t width, uint32_t height) {
	cryfmtlog_info(LogTag::Gfx, "Graphics device: {}\n", StringConverter::ConvertTo<std::string>(RHICore::get_physical_device_description()));
	m_swapChain = std::make_unique<SwapChain>(m_hWnd);

	m_clientWidth  = width;
	m_clientHeight = height;
		
	Resize(m_clientWidth, m_clientHeight);

	m_isIntialized = true;
}

void Graphics::Resize(USize size) {
	Resize(size.Width, size.Height);
}

void Crystal::Graphics::Resize(uint32_t width, uint32_t height) {
	if (m_clientWidth != width || m_clientHeight != height) [[likely]] {
		m_clientWidth  = std::max(1u, width);
		m_clientHeight = std::max(1u, height);

		m_viewPort = {
			.TopLeftX = 0.0f,
			.TopLeftY = 0.0f,
			.Width    = static_cast<float>(m_clientWidth),
			.Height   = static_cast<float>(m_clientHeight),
			.MinDepth = 0.0f,
			.MaxDepth = 0.0f
		};

		m_camera.SetProjection(45.0f, m_viewPort.AspectRatio(), {.Near = 0.1f, .Far = 1000.0f });

		m_renderTarget.Resize(m_clientWidth, m_clientHeight);
		m_swapChain->Resize(m_clientWidth, m_clientHeight);
	}
}
