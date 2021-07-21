#pragma once
#include <iostream>
#include <mutex>
#include <string>
#include <format>
#include <memory>
#include <unordered_map>
#include <variant>

#include "Sink.h"
#include "LogLevels.h"

namespace Crystal {
	class Logger {
	public:
		[[nodiscard]] static Logger& Get() noexcept;
		[[nodiscard]] static constexpr auto NewLine() noexcept { return std::endl<char, std::char_traits<char>>; }

		static void AttachSink(const std::shared_ptr<ISinkBase>& sink) noexcept;
		static void RemoveSink(const std::string_view name) noexcept;

		constexpr void Log(LogLevel lvl, const std::source_location& loc, auto&&... args) noexcept;
		constexpr void FormatLog(LogLevel lvl, const std::source_location& loc, std::string_view fmt, auto&&... args) noexcept;
	private:
		Logger()                             = default;
		Logger(const Logger& rhs)            = delete;
		Logger& operator=(const Logger& rhs) = delete;
		Logger(Logger&& rhs)                 = delete;
		Logger& operator=(Logger&& rhs)      = delete;
		~Logger()                            = default;

		constexpr void InternalLog(LogLevel lvl, const std::source_location& loc, auto&&... args) const noexcept;
		constexpr void InternalFormatLog(LogLevel lvl, const std::source_location& loc, std::string_view fmt, auto&&... args) const noexcept;

		std::mutex m_loggingMutex;
		static std::mutex m_sinkMutex;

		std::unordered_map<std::string_view, std::shared_ptr<ISinkBase>> m_sinks;
	};

	inline constexpr void Logger::Log(LogLevel lvl, const std::source_location& loc, auto&&... args) noexcept {
		std::scoped_lock lock(m_loggingMutex);
		InternalLog(lvl, loc, std::forward<decltype(args)>(args)...);
	}

	inline constexpr void Logger::InternalLog(LogLevel lvl, const std::source_location& loc, auto&&... args) const noexcept {
		const std::string& message = (std::stringstream{} << ... << args).str();

		for (const auto& [key, sink] : m_sinks) {
			sink->Emit(message, lvl, loc);
		}
	}

	inline constexpr void Logger::FormatLog(LogLevel lvl, const std::source_location& loc, std::string_view fmt, auto&&... args) noexcept {
		std::scoped_lock lock(m_loggingMutex);
		InternalFormatLog(lvl, loc, fmt, std::forward<decltype(args)>(args)...);
	}

	inline constexpr void Logger::InternalFormatLog(LogLevel lvl, const std::source_location& loc, std::string_view fmt, auto&&... args) const noexcept {
		std::string message = std::format(fmt, std::forward<decltype(args)>(args)...);

		for (const auto& [key, sink] : m_sinks) {
			sink->Emit(message, lvl, loc);
		}
	}
}

#define crylog_info(...)    Logger::Get().Log(LogLevel::info, std::source_location::current(), __VA_ARGS__)
#define crylog_warning(...) Logger::Get().Log(LogLevel::warning, std::source_location::current(), __VA_ARGS__)
#define crylog_error(...)   Logger::Get().Log(LogLevel::error, std::source_location::current(), __VA_ARGS__)

#define cryfmtlog_info(fmt, ...)    Logger::Get().FormatLog(LogLevel::info, std::source_location::current(), fmt, __VA_ARGS__)
#define cryfmtlog_warning(fmt, ...) Logger::Get().FormatLog(LogLevel::warning, std::source_location::current(), fmt, __VA_ARGS__)
#define cryfmtlog_error(fmt, ...)   Logger::Get().FormatLog(LogLevel::error, std::source_location::current(), fmt, __VA_ARGS__)
