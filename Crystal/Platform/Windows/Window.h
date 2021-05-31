#include "CrystalWindow.h"
#include <vector>
#include <cstdint>
#include <string>
#include "../Core/Input/Keyboard.h"
#include "../Core/Input/Mouse.h"

namespace Crystal {
	enum class MouseButton { Left, Right };

	struct WindowInfo {
		HWND HWnd;
		RECT WindowRect;
		int Width{ 1300 };
		int Height{ 800 };
		std::wstring Name = L"Crystal";
	};

	struct MouseInfo {
		int LastPosX{};
		int LastPosY{};
	};

	class Window {
	public:
		explicit Window(const WindowInfo& info) noexcept;
		Window() noexcept;
		Window(const Window&)            = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&)                 = delete;
		Window& operator=(Window&&)      = delete;
		~Window();

		[[nodiscard]] HWND GetWindowHandle() const { return m_windowInfo.HWnd; }
		[[nodiscard]] uint32_t GetWidth() const noexcept { return m_windowInfo.Width; }
		[[nodiscard]] uint32_t GetHeight() const noexcept { return m_windowInfo.Height; }
		void ToggleFullScreen(bool fullscreen) noexcept;
		bool FullScreen() { return m_fullScreen; }
		void Show() noexcept;

		void MouseMove(LPARAM lParam, WPARAM wParam) noexcept;
		void MouseWheel(LPARAM lParam, WPARAM wParam) noexcept;
		void MouseDown(LPARAM lParam, MouseButton buttonClicked) noexcept;
		void MouseUp(LPARAM lParam, MouseButton buttonClicked) noexcept;
		void RawMouseInput(LPARAM lParam) noexcept;

		bool CursorEnabled() const noexcept { return m_mouse.cursor.m_cursorEnabled; }
		void EnableCursor() noexcept { m_mouse.cursor.Enable(); }
		void DisableCursor() noexcept { m_mouse.cursor.Disable(m_windowInfo.HWnd); }

		Keyboard Kbd;
		Mouse m_mouse;
	private:
		class WindowClass {
		public:
			static const wchar_t* GetName() noexcept;
			static HINSTANCE GetInstance() noexcept;
		private:
			WindowClass();
			~WindowClass();
			WindowClass(const WindowClass&) = delete;
			WindowClass& operator = (const WindowClass&) = delete;
			static constexpr const wchar_t* wndClassName = L"Crystal Window";
			static WindowClass wndClass;
			HINSTANCE hInst;
		};

		static LRESULT CALLBACK SetupProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		static LRESULT CALLBACK RedirectProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

		void CreateMainWindow() noexcept;
		void Resize() noexcept;

		WindowInfo m_windowInfo;
		MouseInfo m_mouseInfo;
		bool m_fullScreen;
		std::vector<std::byte> m_rawInputBuffer;
	};
}