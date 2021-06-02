#pragma once
#include <optional>
#include <memory>

namespace Crystal {
    struct ApplicationCreateInfo;
    class Window;
    class Application {
    public:
        explicit Application(const ApplicationCreateInfo& info);
        Application(const Application& rhs)            = delete;
        Application& operator=(const Application& rhs) = delete;
        Application(Application&& rhs)                 = delete;
        Application& operator=(Application&& rhs)      = delete;
        ~Application();

        Window& GetWindow() const noexcept;
    private:
        void Initialize() noexcept;
        void HandleInput() noexcept;
        void KeyboardInput() noexcept;
        void MouseInput() noexcept;

        std::unique_ptr<Window> m_window = nullptr;

        bool m_isInitialized = false;
    };
}