#include "Window.h"
#include <comdef.h>

namespace Crystal {
	Window::WindowClass Window::WindowClass::wndClass;

	Window::WindowClass::WindowClass()
		:
		hInst(GetModuleHandle(nullptr)) 
	{
		HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (FAILED(hr)) [[unlikely]] {
			//It is not a good sign if we are in this branch
			_com_error err(hr);
			auto ErrorMsg = std::wstring(err.ErrorMessage());

			//throw std::exception(ErrorMsg.c_str());
		}

		WNDCLASSEX wc = {
			.cbSize        = sizeof(wc),
		    .style         = CS_OWNDC,
		    .lpfnWndProc   = SetupProc,
		    .cbClsExtra    = 0,
		    .cbWndExtra    = 0,
		    .hInstance     = GetInstance(),
		    .hIcon         = nullptr,
		    .hbrBackground = nullptr,
		    .lpszMenuName  = nullptr,
		    .lpszClassName = GetName(),
		    .hIconSm       = nullptr
		};

		ATOM atom = RegisterClassExW(&wc);
		assert(atom > 0);
	}

	Window::WindowClass::~WindowClass() {
		UnregisterClassW(wndClassName, GetInstance());
	}

	const wchar_t* Window::WindowClass::GetName() noexcept { return wndClassName; }
	HINSTANCE Window::WindowClass::GetInstance() noexcept { return wndClass.hInst; }

	Window::Window(const WindowInfo& info) noexcept
		:
		m_windowInfo(info)
	{
		CreateMainWindow();
	}

	Window::Window() noexcept {
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
				HMONITOR hMonitor = MonitorFromWindow(m_windowInfo.HWnd, MONITOR_DEFAULTTONEAREST);

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
		if (!m_mouse.cursor.IsEnabled()) {
			if (!m_mouse.IsInWindow()) {
				SetCapture(m_windowInfo.HWnd);
				m_mouse.OnMouseEnter();
				m_mouse.cursor.Hide();
			}
			return;
		}

		const auto point = MAKEPOINTS(lParam);
		int x = point.x;
		int y = point.y;

		int deltaX = x - m_mouseInfo.LastPosX;
		int deltaY = y - m_mouseInfo.LastPosY;

		m_mouse.m_deltaX = deltaX;
		m_mouse.m_deltaY = deltaY;

		m_mouseInfo.LastPosX = x;
		m_mouseInfo.LastPosY = y;

		if (x >= 0 && x <= m_windowInfo.Width && y >= 0 && y < m_windowInfo.Height) {
			m_mouse.OnMouseMove(x, y);

			if (!m_mouse.IsInWindow()) {
				SetCapture(m_windowInfo.HWnd);
				m_mouse.OnMouseEnter();
			}
		}

		else {
			if (wParam & (MK_LBUTTON | MK_RBUTTON)) {
				m_mouse.OnMouseMove(x, y);
			}
			else {
				ReleaseCapture();
				m_mouse.OnMouseLeave();
			}
		}
	}

	void Window::MouseWheel(LPARAM lParam, WPARAM wParam) noexcept {
		const auto point = MAKEPOINTS(lParam);
		const int delta  = GET_WHEEL_DELTA_WPARAM(wParam);

		m_mouse.OnWheelDelta(point.x, point.y, delta);
	}

	void Window::MouseDown(LPARAM lParam, MouseButton buttonClicked) noexcept {
		const auto point = MAKEPOINTS(lParam);

		if (buttonClicked == MouseButton::Left) {
			SetForegroundWindow(m_windowInfo.HWnd);

			if (!m_mouse.cursor.m_cursorEnabled) {
				m_mouse.cursor.Confine(m_windowInfo.HWnd);
				m_mouse.cursor.Hide();
			}

			m_mouse.OnLeftPressed(point.x, point.y);
		}
		else {
			m_mouse.OnRightPressed(point.x, point.y);
		}
	}

	void Window::MouseUp(LPARAM lParam, MouseButton buttonClicked) noexcept {
		const auto point = MAKEPOINTS(lParam);
		int x = point.x;
		int y = point.y;

		(buttonClicked == MouseButton::Left)
			?
			m_mouse.OnLeftReleased(point.x, point.y)
			:
			m_mouse.OnRightReleased(point.x, point.y);

		if (x < 0 || x >= m_windowInfo.Width || y < 0 || y >= m_windowInfo.Height) {
			ReleaseCapture();
			m_mouse.OnMouseLeave();
		}
	}

	void Window::RawMouseInput(LPARAM lParam) noexcept {
		if (!m_mouse.m_rawEnabled) {
			return;
		}

		uint32_t size;
		auto hRawInput = reinterpret_cast<HRAWINPUT>(lParam);

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
		const auto& rawInput = reinterpret_cast<const RAWINPUT&>(*m_rawInputBuffer.data());
		int dx = rawInput.data.mouse.lLastX;
		int dy = rawInput.data.mouse.lLastY;

		if (rawInput.header.dwType == RIM_TYPEMOUSE && (dx != 0 || dy != 0)) {
			m_mouse.OnRawDelta(dx, dy);
		}
	}

	LRESULT Window::SetupProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
		if (msg == WM_NCCREATE) {
			const CREATESTRUCTW* const pCreateStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Window* const pWnd                       = static_cast<Window*>(pCreateStruct->lpCreateParams);

			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::RedirectProc));

			return pWnd->MsgProc(hwnd, msg, wParam, lParam);
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	LRESULT Window::RedirectProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
		Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		return pWnd->MsgProc(hwnd, msg, wParam, lParam);
	}

	LRESULT Window::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
		switch (msg) {
		case WM_ACTIVATE:
			if (!m_mouse.cursor.IsEnabled()) {
				if (wParam & WA_ACTIVE) {
					m_mouse.cursor.Confine(m_windowInfo.HWnd);
					m_mouse.cursor.Hide();
				}
				else {
					m_mouse.cursor.Free();
					m_mouse.cursor.Show();
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
		}
		return DefWindowProcW(hWnd, msg, wParam, lParam);
	}

	void Window::CreateMainWindow() noexcept {
		RECT wndRect = { 0,0, m_windowInfo.Width, m_windowInfo.Height };
		AdjustWindowRect(&wndRect, WS_OVERLAPPED, false);

		int width  = wndRect.right - wndRect.left;
		int height = wndRect.bottom - wndRect.top;

		m_windowInfo.HWnd = CreateWindow(
			Window::WindowClass::GetName(),
			m_windowInfo.Name.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			width,
			height,
			nullptr,
			nullptr,
			Window::WindowClass::GetInstance(),
			this);

		if (!m_windowInfo.HWnd) [[unlikely]] {
			return;
		}

		RAWINPUTDEVICE rawInputDevice{
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
}
