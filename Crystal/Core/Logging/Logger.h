#pragma once
#include <concepts>
#include <format>
#include <memory>
#include <mutex>
#include <source_location>
#include <string>

#include "LogLevels.h"
#include "Sink.h"
#include "Core/Utils/LogUtils.h"

namespace Crystal{
    namespace detail{
        struct log_fmt {
            template<class T>
            log_fmt(T&& msg, std::source_location loc = std::source_location::current())
                :
                msg{ std::forward<T>(msg) },
                loc{loc}
            {}

            std::string_view msg;
            std::source_location loc;
        };
    }

    class Logger {
    public:
        Logger(const Logger& rhs) = delete;
        Logger& operator=(const Logger& rhs) = delete;
        Logger(Logger&& rhs) = delete;
        Logger& operator=(Logger&& rhs) = delete;

        [[nodiscard]] static Logger& Get() noexcept {
            static Logger logger;
            return logger;
        }

        [[nodiscard]] static constexpr auto EndLine() noexcept { return std::endl<char, std::char_traits<char>>; }
        [[nodiscard]] static constexpr auto NewLine() noexcept { return "\n"; }

        template<std::derived_from<ISink> T>
        static void AddSink(auto&&... args) noexcept
            requires std::constructible_from<T, decltype(args)...> {
            std::scoped_lock lock(Get().m_sinkMutex);
            Get().m_sinks.emplace_back(std::make_unique<T>(std::forward<decltype(args)>(args)...));
        }

        static void AddSink(std::unique_ptr<ISink>&& sink) noexcept {
            std::scoped_lock lock(Get().m_sinkMutex);
            Get().m_sinks.emplace_back(std::move(sink));
        }

        template <std::derived_from<ISink> T>
        static void RemoveSink() noexcept {
            std::scoped_lock lock(Get().m_sinkMutex);
            std::erase_if(Get().m_sinks, [](const auto& sink) { return typeid(*sink) == typeid(T); });
        }

        static constexpr void SetDefaultTag(std::string_view newTag) noexcept { Get().m_tag = newTag; }
        static constexpr std::string_view GetDefaultTag() noexcept { return Get().m_tag; }

        constexpr void Log(LogLevel lvl, const detail::log_fmt& fmt, auto&&... args) const noexcept {
            std::scoped_lock lock(m_loggingMutex);

            const std::string message = std::format("{}{}", m_tag, std::format(fmt.msg, std::forward<decltype(args)>(args)...));

            for (const auto& sink : m_sinks) {
                sink->Emit(message, lvl, fmt.loc);
            }
        }

        static constexpr void Info(detail::log_fmt fmt, auto&& args...) {
            Get().Log(LogLevel::info, fmt, std::forward<decltype(args)>(args));
        }

        static constexpr void Info(std::string_view msg, std::source_location loc = std::source_location::current()) {
            Info(detail::log_fmt{"{}", loc}, msg);
        }

        static constexpr void Warning(detail::log_fmt fmt, auto&& args...) {
            Get().Log(LogLevel::warning, fmt, std::forward<decltype(args)>(args));
        }

        static constexpr void Warning(std::string_view msg,
                                      std::source_location loc = std::source_location::current()) {
            Warning(detail::log_fmt{"{}", loc}, msg);
        }

        static constexpr void Error(detail::log_fmt fmt, auto&& args...) {
            Get().Log(LogLevel::error, fmt, std::forward<decltype(args)>(args));
        }

        static constexpr void Error(std::string_view msg, std::source_location loc = std::source_location::current()) {
            Error(detail::log_fmt{"{}", loc}, msg);
        }

        static constexpr void Debug(detail::log_fmt fmt, auto&& args...) {
#if _DEBUG
            Get().Log(LogLevel::debug, fmt, std::forward<decltype(args)>(args));
#endif
        }

        static constexpr void Debug(std::string_view msg, std::source_location loc = std::source_location::current()) {
            Debug(detail::log_fmt{"{}", loc}, msg);
        }

        static void Trace(detail::log_fmt fmt, auto&& args...) {
            auto& logger = Get();

            logger.m_tag.append(std::format(" {}", log_utils::parse_log_tag(fmt.loc.file_name())));
            logger.Log(LogLevel::trace, fmt, std::forward<decltype(args)>(args));
        }

        static void Trace(std::string_view msg, std::source_location loc = std::source_location::current()) {
            Trace(detail::log_fmt{"{}", loc}, msg);
        }

    private:
        Logger() = default;
        ~Logger() = default;

        mutable std::mutex m_loggingMutex;
        std::mutex m_sinkMutex;
        std::vector<std::unique_ptr<ISink>> m_sinks;
        std::string m_tag;
    };
}
