#include "Window.h"
#include <comdef.h>
#include "Core/Utils/StringUtils.h"

namespace Crystal {
	Window::WindowClass Window::WindowClass::wndClass;

	Window::WindowClass::WindowClass()
		:
		hInst(GetModuleHandle(nullptr)) 
	{
		/*HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (FAILED(hr)) [[unlikely]] {
			_com_error err(hr);
			const auto ErrorMsg = StringConverter::ConvertTo<std::string>(err.ErrorMessage());

			throw std::exception(ErrorMsg.c_str());
		}*/

		const WNDCLASSEX wc = {
			.cbSize        = sizeof(wc),
		    .style         = CS_HREDRAW | CS_VREDRAW,
		    .lpfnWndProc   = SetupProc,
		    .cbClsExtra    = 0,
		    .cbWndExtra    = 0,
		    .hInstance     = GetInstance(),
		    .hIcon         = LoadIcon(nullptr, IDI_APPLICATION),
			.hCursor       = LoadCursor(nullptr, IDC_ARROW),
		    .hbrBackground = CreateSolidBrush(RGB(176, 196, 222)),
		    .lpszMenuName  = nullptr,
		    .lpszClassName = GetName(),
		    .hIconSm       = LoadIcon(nullptr, IDI_APPLICATION)
		};

		const ATOM atom = RegisterClassExW(reinterpret_cast<const WNDCLASSEXW*>(&wc));
		assert(atom > 0);
	}

	Window::WindowClass::~WindowClass() {
		UnregisterClassW(WND_CLASS_NAME, GetInstance());
	}

	const wchar_t* Window::WindowClass::GetName() noexcept { return WND_CLASS_NAME; }
	HINSTANCE Window::WindowClass::GetInstance() noexcept { return wndClass.hInst; }

	Window::Window(const ApplicationCreateInfo& info) noexcept
		:
		m_windowInfo(info)
	{
		CreateMainWindow();
	}

	Window::~Window() {
		DestroyWindow(m_windowInfo.HWnd);
	}

	void Window::ToggleFullScreen(bool fullscreen) noexcept {
		if (m_fullScreen != fullscreen) {
			m_fullScreen = fullscreen;

			auto& windowRect = m_windowInfo.WindowRect;

			if (m_fullScreen) {
				GetWindowRect(m_windowInfo.HWnd, &windowRect);

				SetWindowLong(m_windowInfo.HWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

				// Query the name of the nearest display device for the window.
				// This is required to set the fullscreen dimensions of the window
				// when using a multi-monitor setup.
                const auto hMonitor = MonitorFromWindow(m_windowInfo.HWnd, MONITOR_DEFAULTTONEAREST);

				MONITORINFOEX monitorInfo = {};
				monitorInfo.cbSize        = sizeof(MONITORINFOEX);

				GetMonitorInfo(hMonitor, &monitorInfo);

				SetWindowPos(m_windowInfo.HWnd, HWND_TOP,
					monitorInfo.rcMonitor.left,
					monitorInfo.rcMonitor.top,
					monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
					monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
					SWP_FRAMECHANGED | SWP_NOACTIVATE);

				ShowWindow(m_windowInfo.HWnd, SW_MAXIMIZE);
			}
			else {
				SetWindowLong(m_windowInfo.HWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

				SetWindowPos(m_windowInfo.HWnd, HWND_NOTOPMOST,
					windowRect.left,
					windowRect.top,
					windowRect.right - windowRect.left,
					windowRect.bottom - windowRect.top,
					SWP_FRAMECHANGED | SWP_NOACTIVATE);

				ShowWindow(m_windowInfo.HWnd, SW_NORMAL);
			}
		}
	}

	void Window::Show() noexcept {
		ShowWindow(m_windowInfo.HWnd, SW_SHOWNORMAL);
		UpdateWindow(m_windowInfo.HWnd);
	}

	void Window::MouseMove(LPARAM lParam, WPARAM wParam) noexcept {
		if (!Mouse.cursor.IsEnabled()) {
			if (!Mouse.IsInWindow()) {
				SetCapture(m_windowInfo.HWnd);
				Mouse.OnMouseEnter();
				Mouse::Cursor::Hide();
			}
			return;
		}

		const auto [x, y] = MAKEPOINTS(lParam);

		const int deltaX = x - m_mouseInfo.LastPosX;
		const int deltaY = y - m_mouseInfo.LastPosY;

		Mouse.m_deltaX = deltaX;
		Mouse.m_deltaY = deltaY;

		m_mouseInfo.LastPosX = x;
		m_mouseInfo.LastPosY = y;

		if (x >= 0 && x <= m_windowInfo.Width && y >= 0 && y < m_windowInfo.Height) {
			Mouse.OnMouseMove(x, y);

			if (!Mouse.IsInWindow()) {
				SetCapture(m_windowInfo.HWnd);
				Mouse.OnMouseEnter();
			}
		}

		else {
			if (wParam & (MK_LBUTTON | MK_RBUTTON)) {
				Mouse.OnMouseMove(x, y);
			}
			else {
				ReleaseCapture();
				Mouse.OnMouseLeave();
			}
		}
	}

	void Window::MouseWheel(LPARAM lParam, WPARAM wParam) noexcept {
		const auto [x, y] = MAKEPOINTS(lParam);
		const int delta  = GET_WHEEL_DELTA_WPARAM(wParam);

		Mouse.OnWheelDelta(x, y, delta);
	}

	void Window::MouseDown(LPARAM lParam, MouseButton buttonClicked) noexcept {
		const auto [x, y] = MAKEPOINTS(lParam);

		if (buttonClicked == MouseButton::Left) {
			SetForegroundWindow(m_windowInfo.HWnd);

			if (!Mouse.cursor.m_cursorEnabled) {
				Mouse::Cursor::Confine(m_windowInfo.HWnd);
				Mouse::Cursor::Hide();
			}

			Mouse.OnLeftPressed(x, y);
		}
		else {
			Mouse.OnRightPressed(x, y);
		}
	}

	void Window::MouseUp(LPARAM lParam, MouseButton buttonClicked) noexcept {
		const auto [x, y] = MAKEPOINTS(lParam);

		(buttonClicked == MouseButton::Left)
			?
			Mouse.OnLeftReleased(x, y)
			:
			Mouse.OnRightReleased(x, y);

		if (x < 0 || x >= m_windowInfo.Width || y < 0 || y >= m_windowInfo.Height) {
			ReleaseCapture();
			Mouse.OnMouseLeave();
		}
	}

	void Window::RawMouseInput(LPARAM lParam) noexcept {
		if (!Mouse.m_rawEnabled) {
			return;
		}

		uint32_t size{};
		const auto hRawInput = reinterpret_cast<HRAWINPUT>(lParam);

		//Get the size of the data
		uint32_t result = GetRawInputData(hRawInput, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

		if (result == -1) {
			return;
		}
		m_rawInputBuffer.resize(size);

		//read the data
		result = GetRawInputData(hRawInput, RID_INPUT, m_rawInputBuffer.data(), &size, sizeof(RAWINPUTHEADER));

		if (result != size) {
			return;
		}

		//Process the data
		const auto& [header, data] = reinterpret_cast<const RAWINPUT&>(*m_rawInputBuffer.data());
		
		const int dx = data.mouse.lLastX;
		const int dy = data.mouse.lLastY;

		if (header.dwType == RIM_TYPEMOUSE && (dx != 0 || dy != 0)) {
			Mouse.OnRawDelta(dx, dy);
		}
	}

	LRESULT Window::SetupProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
		if (msg == WM_NCCREATE) {
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			auto* const pWnd                   = static_cast<Window*>(pCreate->lpCreateParams);

			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::RedirectProc));

			return pWnd->MsgProc(hwnd, msg, wParam, lParam);
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	LRESULT Window::RedirectProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
		auto* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		return pWnd->MsgProc(hwnd, msg, wParam, lParam);
	}

	LRESULT Window::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
		switch (msg) {
		case WM_ACTIVATE:
			if (!CursorEnabled()) {
				if (wParam & WA_ACTIVE) {
					Mouse.cursor.Confine(m_windowInfo.HWnd);
					Mouse.cursor.Hide();
				}
				else {
					Mouse.cursor.Free();
					Mouse.cursor.Show();
				}
			}
			break;
		case WM_KILLFOCUS:
			Kbd.ClearState();
			break;
			//Keyboard messages
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			if (!(lParam & 0x40000000) || Kbd.AutorepeatIsEnabled()) {
				Kbd.OnKeyPressed(static_cast<KeyCode>(wParam));
			}
			break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
			Kbd.OnKeyReleased(static_cast<KeyCode>(wParam));
			break;
		case WM_CHAR:
			Kbd.OnChar(static_cast<unsigned char>(wParam));
			break;
		case WM_MOUSEWHEEL:
			MouseWheel(lParam, wParam);
			break;
			//Mouse messages
		case WM_MOUSEMOVE:
			MouseMove(lParam, wParam);
			break;
		case WM_LBUTTONDOWN:
			MouseDown(lParam, MouseButton::Left);
			break;
		case WM_RBUTTONDOWN:
			MouseDown(lParam, MouseButton::Right);
			break;
		case WM_LBUTTONUP:
			MouseUp(lParam, MouseButton::Left);
			break;
		case WM_RBUTTONUP:
			MouseUp(lParam, MouseButton::Right);
			break;
			//Raw mouse input
		case WM_INPUT:
			RawMouseInput(lParam);
			break;
		case WM_SIZE:
			Resize();
			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
        default:
                return DefWindowProcW(hWnd, msg, wParam, lParam);
                break;
		}
		return DefWindowProcW(hWnd, msg, wParam, lParam);
	}

	void Window::CreateMainWindow() noexcept {
		m_windowInfo.Style |= m_windowInfo.ParentHwnd ? WS_CHILD : WS_OVERLAPPEDWINDOW;

		RECT wndRect = { 0,0, static_cast<LONG>(m_windowInfo.Width), static_cast<LONG>(m_windowInfo.Height) };
		AdjustWindowRect(&wndRect, m_windowInfo.Style, false);

		const int width  = wndRect.right - wndRect.left;
		const int height = wndRect.bottom - wndRect.top;
		
		m_windowInfo.WindowRect = wndRect;

		m_windowInfo.HWnd = CreateWindow(
			Window::WindowClass::GetName(),
			L"Cyrex",
			m_windowInfo.Style,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			width,
			height,
			m_windowInfo.ParentHwnd,
			nullptr,
			Window::WindowClass::GetInstance(),
			this);

		if (!m_windowInfo.HWnd) [[unlikely]] {
			return;
		}

		const RAWINPUTDEVICE rawInputDevice{
			.usUsagePage = 0x01,
			.usUsage     = 0x02,
			.dwFlags     = 0,
			.hwndTarget  = nullptr
		};

		RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice));
	}

	void Window::Resize() noexcept {
		RECT rect;
		GetClientRect(m_windowInfo.HWnd, &rect);

		m_windowInfo.Width  = rect.right - rect.left;
		m_windowInfo.Height = rect.bottom - rect.top;
	}

    std::optional<int> Window::MessagePump() noexcept {
        MSG msg{};

        while(PeekMessageA(&msg, nullptr, 0u,0u,PM_REMOVE)) {
            if(msg.message == WM_QUIT) {
                return msg.wParam;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return {};
    }
}
