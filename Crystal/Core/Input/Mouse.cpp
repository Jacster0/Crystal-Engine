#include "Mouse.h"

namespace Crystal {
    void Mouse::Cursor::Enable() noexcept {
        m_cursorEnabled = true;
        Show();
        Free();
    }

    void Mouse::Cursor::Disable(HWND hWnd) noexcept {
        m_cursorEnabled = false;
        Hide();
        Confine(hWnd);
    }

    bool Mouse::Cursor::IsEnabled() const noexcept { return m_cursorEnabled; }

    void Mouse::Cursor::Confine(HWND hWnd) noexcept {
        RECT rect;
        GetClientRect(hWnd, &rect);
        MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
        ClipCursor(&rect);
    }

    void Mouse::Cursor::Free() noexcept {
        ClipCursor(nullptr);
    }

    void Mouse::Cursor::Show() noexcept {
        while (::ShowCursor(TRUE) < 0);
    }

    void Mouse::Cursor::Hide() noexcept {
        while (::ShowCursor(FALSE) >= 0);
    }

    void Mouse::EnableRawInput() noexcept { m_rawEnabled = true; }

    void Mouse::DisableRawInput() noexcept { m_rawEnabled = false; }

    void Mouse::OnMouseMove(int x, int y) noexcept {
        m_x = x;
        m_y = y;

        m_buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
        TrimBuffer();
    }

    void Mouse::OnMouseLeave() noexcept {
        m_isInWindow = false;

        m_buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
        TrimBuffer();
    }

    void Mouse::OnMouseEnter() noexcept {
        m_isInWindow = true;

        m_buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
        TrimBuffer();
    }

    void Mouse::OnRawDelta(int dx, int dy) noexcept {
        m_rawDeltaBuffer.push({ dx,dy });
        TrimRawInputBuffer();
    }

    void Mouse::OnLeftPressed(int x, int y) noexcept {
        m_leftIsPressed = true;

        m_buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
        TrimBuffer();
    }

    void Mouse::OnLeftReleased(int x, int y) noexcept {
        m_leftIsPressed = false;

        m_buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
        TrimBuffer();
    }

    void Mouse::OnRightPressed(int x, int y) noexcept {
        m_rightIsPressed = true;

        m_buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
        TrimBuffer();
    }

    void Mouse::OnRightReleased(int x, int y) noexcept {
        m_rightIsPressed = false;

        m_buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
        TrimBuffer();
    }

    void Mouse::OnWheelUp(int x, int y) noexcept {
        m_buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
        TrimBuffer();
    }

    void Mouse::OnWheelDown(int x, int y) noexcept {
        m_buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
        TrimBuffer();
    }

    void Mouse::TrimBuffer() noexcept {
        while (m_buffer.size() > m_bufferSize) {
            m_buffer.pop();
        }
    }

    std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept {
        if (m_rawDeltaBuffer.empty()) {
            return std::nullopt;
        }

        const RawDelta d = m_rawDeltaBuffer.front();
        m_rawDeltaBuffer.pop();
        return d;
    }

    void Mouse::TrimRawInputBuffer() noexcept {
        while (m_rawDeltaBuffer.size() > m_bufferSize) {
            m_rawDeltaBuffer.pop();
        }
    }

    void Mouse::OnWheelDelta(int x, int y, int delta) noexcept {
        m_wheelDelta = delta / static_cast<int>(WHEEL_DELTA);

        (m_wheelDelta < 0) ? OnWheelUp(x, y) : OnWheelDown(x, y);
    }

    std::pair<int, int> Mouse::GetPos() const noexcept {
        return { m_x,m_y };
    }

    int Mouse::GetPosX() const noexcept {
        return m_x;
    }

    int Mouse::GetPosY() const noexcept {
        return m_y;
    }

    int Mouse::GetDeltaX() const noexcept {
        return m_deltaX;
    }

    int Mouse::GetDeltaY() const noexcept {
        return m_deltaY;
    }

    bool Mouse::LeftIsPressed() const noexcept {
        return m_leftIsPressed;
    }

    bool Mouse::RightIsPressed() const noexcept {
        return m_rightIsPressed;
    }

    bool Mouse::IsInWindow() const noexcept {
        return m_isInWindow;
    }

    std::optional<Mouse::Event> Mouse::Read() noexcept {
        if (m_buffer.size() > 0u) {
            Mouse::Event e = m_buffer.front();
            m_buffer.pop();

            return e;
        }
        return {};
    }

    void Mouse::Flush() noexcept {
        m_buffer = std::queue<Event>();
    }
}
