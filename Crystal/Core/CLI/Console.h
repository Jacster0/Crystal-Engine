#pragma once

#include <cstdint>
#include <string_view>

enum class ConsoleColor : std::uint16_t {
    Red    = (0x0004 | 0x0008),
    Green  = (0x0002 | 0x0008),
    Yellow = (0x0004 | 0x0002 | 0x0008),
    White  = (0x0004 | 0x0002 | 0x0001),
};

namespace Console {
    void Create() noexcept;
    void Destroy() noexcept;

    void Show() noexcept;
    void Hide() noexcept;
    [[nodiscard]] bool Visible() noexcept;

    void Write(std::string_view msg);
    void WriteLine(std::string_view msg) noexcept;
    void SetTextColor(ConsoleColor clr) noexcept;
};


