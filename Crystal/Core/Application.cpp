#include "Application.h"
#include "Logging/Logger.h"
#include "Logging/ManagedLoggerSink.h"

#include "../Platform/Windows/Window.h"
#include "../Platform/Windows/Types.h"
#include "../RHI/RHICore.h"
#include "../Graphics/Graphics.h"

namespace Crystal {
	Application::Application(const ApplicationCreateInfo& info) 
	{
		m_window = std::make_unique<Window>(info);
		m_gfx = std::make_unique<Graphics>();
		m_gfx->SetWindowHandle(m_window->GetWindowHandle());

		Logger::AddSink<ManagedLoggerSink, "ManagedLogger">();
		m_cpuInfo = CpuInfo{};

		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		Initialize();
	}

	Application::~Application() { }

	Window& Application::GetWindow() const noexcept {
		return *m_window;
	}

	void Application::Run()
	{
		while (true) {
			HandleInput();
		}
	}

	void Application::Initialize() noexcept {
		RHICore::Intialize();
		m_gfx->Initialize(m_window->GetWidth(), m_window->GetHeight());

		m_window->Kbd.EnableAutorepeat();
		m_isInitialized = true;
	}

	void Application::HandleInput() noexcept {
		KeyboardInput();
		MouseInput();
	}

	void Application::KeyboardInput() noexcept {
		while (const auto e = m_window->Kbd.ReadKey()) {
			if (!e->IsPress()) {
				continue;
			}

			switch (e->GetCode()) {
			case KeyCode::Escape:
				crylog_info("Escape pressed");

				if (m_window->CursorEnabled()) {
					m_window->DisableCursor();
					m_window->m_mouse.EnableRawInput();
				}
				else {
					m_window->EnableCursor();
					m_window->m_mouse.DisableRawInput();
				}
				break;
			case KeyCode::F11:
				m_window->ToggleFullScreen(!m_window->FullScreen());
				break;
			case KeyCode::Space:
				crylog_info("Escape pressed");
				break;
			}
		}

		if (m_window->Kbd.KeyIsPressedOnce(KeyCode::V)) {
		}
	}

	void Application::MouseInput() noexcept {
		using enum Mouse::Event::Type;
		while (const auto e = m_window->m_mouse.Read()) {
			switch (e->GetType()) {
			case Move:
				break;
			case WheelUp:
			case WheelDown:
				break;
			}
		}

		while (const auto delta = m_window->m_mouse.ReadRawDelta()) {
			if (!m_window->m_mouse.cursor.IsEnabled()) {
			}
		}
	}
}

