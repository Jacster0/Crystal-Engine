#include "ConsoleSink.h"
#include "Core/CLI/Console.h"
#include "Core/Time/CrystalTimer.h"
#include "Core/Time/Time.h"

using namespace Crystal;

ConsoleSink::ConsoleSink() noexcept {
    m_prefix.append("[").append(current_time()).append("] ");
}

void ConsoleSink::Emit(std::string_view msg, Crystal::LogLevel lvl, const std::source_location &loc) noexcept {
    switch (lvl) {
        case LogLevel::info:
            Console::SetTextColor(ConsoleColor::Green);
            break;
        case LogLevel::warning:
            Console::SetTextColor(ConsoleColor::Yellow);
        case LogLevel::error:
            Console::SetTextColor(ConsoleColor::Red);
            break;
    }

    Console::WriteLine(std::string(m_prefix).append(msg));
    Console::SetTextColor(ConsoleColor::White);
}
