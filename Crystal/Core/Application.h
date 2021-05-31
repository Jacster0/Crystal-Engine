#pragma once
#include <optional>
#include <memory>

namespace Crystal {
    class Window;
    class Application {
    public:
        Application();
        Application(const Application& rhs)            = delete;
        Application& operator=(const Application& rhs) = delete;
        Application(Application&& rhs)                 = delete;
        Application& operator=(Application&& rhs)      = delete;
        ~Application();

        int Run();
    private:
        void Initialize() noexcept;
        void HandleInput() noexcept;
        void KeyboardInput() noexcept;
        void MouseInput() noexcept;
        std::optional<int> MessagePump() noexcept;

        std::unique_ptr<Window> m_window = nullptr;

        bool m_isInitialized = false;
    };
}