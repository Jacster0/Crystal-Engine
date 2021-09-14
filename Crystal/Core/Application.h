#pragma once
#include <optional>
#include <memory>
#include "InstructionSet/CpuInfo.h"

namespace Crystal {
    struct ApplicationCreateInfo;
    class Window;
    class Graphics;
    class Application {
    public:
        explicit Application(const ApplicationCreateInfo& info);
        Application(const Application& rhs)            = delete;
        Application& operator=(const Application& rhs) = delete;
        Application(Application&& rhs)                 = delete;
        Application& operator=(Application&& rhs)      = delete;
        ~Application();

        [[nodiscard]] Window& GetWindow() const noexcept;
        [[nodiscard]] CpuInfo& GetCpuInfo() noexcept { return m_cpuInfo; };

        void Run();
    private:
        void Initialize() noexcept;
        void HandleInput() noexcept;
        void KeyboardInput() noexcept;
        void MouseInput() noexcept;

        std::unique_ptr<Window> m_window{ nullptr };
        std::unique_ptr<Graphics> m_gfx{ nullptr };

        CpuInfo m_cpuInfo;
        bool m_isInitialized = false;
    };
}