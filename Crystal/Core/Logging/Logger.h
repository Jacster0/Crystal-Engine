#pragma once
#include <mutex>
#include <string>
#include <format>
#include <memory>
#include <unordered_map>
#include <source_location>
#include <concepts>

#include "LogLevels.h"
#include "Sink.h"

namespace Crystal {
    namespace LogTag {
        constexpr auto Core = "CrystalCore: ";
        constexpr auto Gfx  = "CrystalGfx: ";
    }
	class Logger {
	public:
        Logger(const Logger& rhs)            = delete;
        Logger& operator=(const Logger& rhs) = delete;
        Logger(Logger&& rhs)                 = delete;
        Logger& operator=(Logger&& rhs)      = delete;

		[[nodiscard]] static Logger& Get() noexcept {
			static Logger logger;
			return logger;
		}

		[[nodiscard]] static constexpr auto EndLine() noexcept { return std::endl<char, std::char_traits<char>>; }
		[[nodiscard]] static constexpr auto NewLine() noexcept { return "\n"; }

		template<std::derived_from<ISink> T>
		static void AddSink(auto&&... args) noexcept
			requires std::constructible_from<T, decltype(args)...>
		{
			std::scoped_lock lock(Logger::Get().m_sinkMutex);
			Logger::Get().m_sinks.emplace_back(std::make_unique<T>(std::forward<decltype(args)>(args)...));
		}

		static void AddSink(std::unique_ptr<ISink>&& sink) noexcept {
			std::scoped_lock lock(Logger::Get().m_sinkMutex);
			Logger::Get().m_sinks.emplace_back(std::move(sink));
		}

		/// <summary>
		/// Removes all sinks of type T that is attached to the Logger
		/// </summary>
		template<std::derived_from<ISink> T>
		static void RemoveSink() noexcept {
			std::scoped_lock lock(Logger::Get().m_sinkMutex);
			std::erase_if(Logger::Get().m_sinks, [](const auto& sink) { return typeid(*sink) == typeid(T); });
		}

		constexpr void Log(std::string_view tag, LogLevel lvl, const std::source_location& loc, auto&&... args) const noexcept {
			std::scoped_lock lock(m_loggingMutex);

			const std::string& message = ((std::stringstream{} << tag) << ... << args).str();

			for (const auto& sink : m_sinks) {
				sink->Emit(message, lvl, loc);
			}
		}

		constexpr void FormatLog(std::string_view tag, LogLevel lvl, const std::source_location& loc, std::string_view fmt, auto&&... args) const noexcept {
			std::scoped_lock lock(m_loggingMutex);

			const std::string message = std::string(tag).append(std::format(fmt, std::forward<decltype(args)>(args)...));

			for (const auto& sink : m_sinks) {
				sink->Emit(message, lvl, loc);
			}
		}
	private:
		Logger()                             = default;
		~Logger()                            = default;

		mutable std::mutex m_loggingMutex;
		std::mutex m_sinkMutex;
		std::vector<std::unique_ptr<ISink>> m_sinks;
	};
}

#define crylog_info(TAG, ...)    Crystal::Logger::Get().Log(TAG, LogLevel::info,    std::source_location::current(), __VA_ARGS__)
#define crylog_warning(TAG, ...) Crystal::Logger::Get().Log(TAG, LogLevel::warning, std::source_location::current(), __VA_ARGS__)
#define crylog_error(TAG, ...)   Crystal::Logger::Get().Log(TAG, LogLevel::error,   std::source_location::current(), __VA_ARGS__)

#define cryfmtlog_info(TAG, fmt, ...)    Crystal::Logger::Get().FormatLog(TAG, LogLevel::info,    std::source_location::current(), fmt, __VA_ARGS__)
#define cryfmtlog_warning(TAG, fmt, ...) Crystal::Logger::Get().FormatLog(TAG, LogLevel::warning, std::source_location::current(), fmt, __VA_ARGS__)
#define cryfmtlog_error(TAG, fmt, ...)   Crystal::Logger::Get().FormatLog(TAG, LogLevel::error,   std::source_location::current(), fmt, __VA_ARGS__)

