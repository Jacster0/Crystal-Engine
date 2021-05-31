#include "Application.h"
#include "../Platform/Windows/Window.h"

namespace Crystal {
	Application::Application() {
		m_window = std::make_unique<Window>();

		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		Initialize();
	}

	Application::~Application() { }

	int Application::Run() {
		if (!m_isInitialized) [[unlikely]] {
			return 1;
		}
		while (true) {
			if (const auto ecode = MessagePump()) {
				//return exit code
				return *ecode;
			}
			else {
				//Render/Graphics stuff
				HandleInput();
			}
		}
	}

	void Application::Initialize() noexcept {
		m_window->Kbd.EnableAutorepeat();
		m_window->Show();

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

	std::optional<int> Application::MessagePump() noexcept {
		MSG msg{ 0 };

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				return msg.wParam;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return {};
	}

}

