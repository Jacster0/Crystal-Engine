#include "Application.h"
#include "../Platform/Windows/Window.h"
#include "../Platform/Windows/Types.h"
#include "../RHI/D3D12/D3D12Core.h"
#include "Logging/Logger.h"
#include "Logging/ManagedLoggerSink.h"

namespace Crystal {
	Application::Application(const ApplicationCreateInfo& info) {
		m_window = std::make_unique<Window>(info);

		m_cpuInfo = CpuInfo{};

		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		Initialize();

		//Testing the logging system
		crylog::info("Test123", " Hello", " World");
		cryfmtlog::info("{0} Test fmt logger\n{1} ", "Test123", "Hello World\n");
	}

	Application::~Application() { }

	Window& Application::GetWindow() const noexcept {
		return *m_window;
	}

	void Application::Initialize() noexcept {
		RHICore::Intialize();

		auto& logger = Logger::Get();
		logger.AttachSink(std::make_shared<ManagedLoggerSink>("ManagedLogger"));

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

