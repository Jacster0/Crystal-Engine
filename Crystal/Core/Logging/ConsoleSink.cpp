#include "ConsoleSink.h"
#include "Core/CLI/Console.h"
#include "Core/Time/CrystalTimer.h"
#include "Core/Time/Time.h"

#include <format>
#include <source_location>

using namespace Crystal;

ConsoleSink::ConsoleSink() noexcept {
    m_prefix.append("[").append(current_time()).append("] ");
}

void ConsoleSink::Emit(std::string_view msg, Crystal::LogLevel lvl, const std::source_location &loc) noexcept {
    std::string msgPrefix{};

    switch (lvl) {
        case LogLevel::info:
            msgPrefix = "<INFO>";
            Console::SetTextColor(ConsoleColor::Green);
            break;
        case LogLevel::warning:
            msgPrefix = "<WARNING>";
            Console::SetTextColor(ConsoleColor::Yellow);
            break;
        case LogLevel::error:
            msgPrefix = "<ERROR>";
            Console::SetTextColor(ConsoleColor::Red);
            break;
        case LogLevel::debug:
            msgPrefix = "<DEBUG>";
            Console::SetTextColor(ConsoleColor::Green);
            break;
        case LogLevel::trace:
            Console::SetTextColor(ConsoleColor::Green);

            msgPrefix = "<TRACE>";
            Console::WriteLine(std::format("{0} {1} {2}({3}) {4}", m_prefix, msgPrefix, loc.file_name(), loc.line(), msg));

            Console::SetTextColor(ConsoleColor::White);
            return;
    }

    Console::WriteLine(std::format("{} {} {}", m_prefix, msgPrefix, msg));
    Console::SetTextColor(ConsoleColor::White);
}
