#pragma once
#include <optional>
#include <queue>
#include "Platform/Windows/CrystalWindow.h"

namespace Crystal {

    class Mouse {
    public:
        class Cursor {
        public:
            void Enable() noexcept;
            void Disable(HWND hWnd) noexcept;
            [[nodiscard]] bool IsEnabled() const noexcept;
            [[nodiscard]] bool IsInWindow() const noexcept { return m_isInWindow; }
        private:
            static void Confine(HWND hWnd) noexcept;
            static void Free() noexcept;
            static void Show() noexcept;
            static void Hide() noexcept;

            bool m_cursorEnabled = true;
            bool m_isInWindow = false;
            friend class Window;
        };

        struct RawDelta {
            int X;
            int Y;
        };

        class Event {
        public:
            enum class Type {
                LPress,
                LRelease,
                RPress,
                RRelease,
                WheelUp,
                WheelDown,
                Move,
                Enter,
                Leave
            };
        private:
            Type type;
            bool leftIsPressed;
            bool rightIsPressed;
            int x;
            int y;
        public:
            Event(Type type, Mouse& mouse)
                :
                type(type),
                leftIsPressed(mouse.m_leftIsPressed),
                rightIsPressed(mouse.m_rightIsPressed),
                x(mouse.m_x),
                y(mouse.m_y)
            {}

            [[nodiscard]] Type GetType() const noexcept { return type; }
            [[nodiscard]] std::pair<int, int> GetPos() const noexcept { return{ x,y }; }

            [[nodiscard]] int GetPosX() const noexcept { return x; }
            [[nodiscard]] int GetPosY() const noexcept { return y; }

            [[nodiscard]] bool LeftIsPressed() const noexcept { return leftIsPressed; }
            [[nodiscard]] bool RightIsPressed() const noexcept { return rightIsPressed; }
        };

        Mouse() = default;
        Mouse(const Mouse&) = delete;
        Mouse& operator=(const Mouse&) = delete;

        [[nodiscard]] int GetWheelDelta() const noexcept { return m_wheelDelta; }
        [[nodiscard]] std::pair<int, int> GetPos() const noexcept;

        [[nodiscard]] int GetPosX() const noexcept;
        [[nodiscard]] int GetPosY() const noexcept;

        [[nodiscard]] int GetDeltaX() const noexcept;
        [[nodiscard]] int GetDeltaY() const noexcept;

        [[nodiscard]] bool LeftIsPressed() const noexcept;
        [[nodiscard]] bool RightIsPressed() const noexcept;
        [[nodiscard]] bool IsInWindow() const noexcept;

        std::optional<Mouse::Event> Read() noexcept;
        [[nodiscard]] bool IsEmpty() const noexcept { return m_buffer.empty(); }
        void Flush() noexcept;

        void EnableRawInput() noexcept;
        void DisableRawInput() noexcept;
        std::optional<RawDelta> ReadRawDelta() noexcept;

        Cursor cursor;
    private:
        void OnMouseMove(int x, int y) noexcept;
        void OnMouseLeave() noexcept;
        void OnMouseEnter() noexcept;

        void OnLeftPressed(int x, int y) noexcept;
        void OnLeftReleased(int x, int y) noexcept;

        void OnRightPressed(int x, int y) noexcept;
        void OnRightReleased(int x, int y) noexcept;

        void OnWheelUp(int x, int y) noexcept;
        void OnWheelDown(int x, int y) noexcept;

        void TrimBuffer() noexcept;
        void TrimRawInputBuffer() noexcept;

        void OnWheelDelta(int x, int y, int delta) noexcept;
        void OnRawDelta(int dx, int dy) noexcept;

        static constexpr unsigned int m_bufferSize = 16u;
        int m_x = 0;
        int m_y = 0;
        float m_deltaX = 0;
        float m_deltaY = 0;
        bool m_leftIsPressed = false;
        bool m_rightIsPressed = false;
        bool m_isInWindow = false;
        int m_wheelDelta = 0;
        bool m_rawEnabled = false;
        std::queue<RawDelta> m_rawDeltaBuffer;
        std::queue<Event> m_buffer;
        friend class Window;
    };
}
