#include "Application.h"
#include "Logging/Logger.h"
#include "Logging/ManagedLoggerSink.h"

#include "../Platform/Windows/Window.h"
#include "../Platform/Windows/Types.h"
#include "../RHI/RHICore.h"
#include "../Graphics/Graphics.h"
#include "Time/Time.h"
#include "Math/MathFunctions.h"

using namespace Crystal;
namespace Crystal {
	Application::Application(const ApplicationCreateInfo& info)
		:
		m_window(std::make_unique<Window>(info)),
		m_gfx(std::make_unique<Graphics>())
	{
		m_gfx->SetWindowHandle(m_window->GetWindowHandle());

		Logger::AddSink<ManagedLoggerSink>();

		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        RHICore::initialize();
		m_gfx->Initialize(m_window->GetWidth(), m_window->GetHeight());
		m_window->Kbd.EnableAutorepeat();

		crylog_info(LogTag::Core, current_date());
		crylog_info(LogTag::Core, current_time());
		crylog_info(LogTag::Core, current_date_time());
		crylog_warning(LogTag::Core, "Test");
	} 

	Application::~Application() { 
		Logger::RemoveSink<ManagedLoggerSink>();
	}

	Window& Application::GetWindow() const noexcept {
		return *m_window;
	}

    [[noreturn]] int Application::Run() const {
		while (true) {
            if(const auto code = Window::MessagePump()) {
                return *code;
            }
			HandleInput();
		}
	}

	void Application::HandleInput() const noexcept {
        KeyboardInput();
        MouseInput();
	}

	void Application::KeyboardInput() const noexcept {
		while (const auto e = m_window->Kbd.ReadKey()) {
			if (!e->IsPress()) {
				continue;
			}

			switch (e->GetCode()) {
			case KeyCode::Escape:
				crylog_info("Escape pressed");

				if (m_window->CursorEnabled()) {
					m_window->DisableCursor();
					m_window->Mouse.EnableRawInput();
				}
				else {
					m_window->EnableCursor();
					m_window->Mouse.DisableRawInput();
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

	void Application::MouseInput() const noexcept {
		using enum Mouse::Event::Type;
		while (const auto e = m_window->Mouse.Read()) {
			switch (e->GetType()) {
			case Move:
				break;
			case WheelUp:
			case WheelDown:
				break;
			}
		}

		while (const auto delta = m_window->Mouse.ReadRawDelta()) {
			if (!m_window->Mouse.cursor.IsEnabled()) {
			}
		}
	}
}

