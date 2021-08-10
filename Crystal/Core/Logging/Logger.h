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
	template<size_t Length>
	struct StringLiteral {
		constexpr StringLiteral(const char* const str) noexcept {
			std::copy_n(str, Length, Value);
		}

		[[nodiscard]] constexpr operator const char* () const noexcept { return Value; }

		char Value[Length + 1]{};
	};

	template<size_t Length>
	StringLiteral(const char(&)[Length])->StringLiteral<Length - 1>;

	class Logger {
	public:
		[[nodiscard]] static Logger& Get() noexcept;
		[[nodiscard]] static constexpr auto NewLine() noexcept { return std::endl<char, std::char_traits<char>>; }

		template<std::derived_from<ISinkBase> T, StringLiteral name>
		static constexpr void AddSink(auto&&... args) noexcept;
		static void AddSink(std::unique_ptr<ISinkBase>&& sink, std::string_view name) noexcept;

		static void RemoveSink(std::string_view name) noexcept;

		constexpr void Log(LogLevel lvl, const std::source_location& loc, auto&&... args) const noexcept;
		constexpr void FormatLog(LogLevel lvl, const std::source_location& loc, std::string_view fmt, auto&&... args) const noexcept;
	private:
		Logger() = default;
		Logger(const Logger& rhs) = delete;
		Logger& operator=(const Logger& rhs) = delete;
		Logger(Logger&& rhs) = delete;
		Logger& operator=(Logger&& rhs) = delete;
		~Logger() = default;

		mutable std::mutex m_loggingMutex;
		static std::mutex m_sinkMutex;
		std::unordered_map<std::string_view, std::unique_ptr<ISinkBase>> m_sinks;
	};

	constexpr void Logger::Log(LogLevel lvl, const std::source_location& loc, auto&& ...args) const noexcept {
		std::scoped_lock lock(m_loggingMutex);

		const std::string& message = (std::stringstream{} << ... << args).str();

		for (const auto& [key, sink] : m_sinks) {
			sink->Emit(message, lvl, loc);
		}
	}

	constexpr void Logger::FormatLog(LogLevel lvl, const std::source_location& loc, std::string_view fmt, auto&&... args) const noexcept {
		std::scoped_lock lock(m_loggingMutex);

		const std::string message = std::format(fmt, std::forward<decltype(args)>(args)...);

		for (const auto& [key, sink] : m_sinks) {
			sink->Emit(message, lvl, loc);
		}
	}

	template<std::derived_from<ISinkBase> T, StringLiteral name>
	constexpr void Logger::AddSink(auto&& ...args) noexcept {
		std::scoped_lock lock(m_sinkMutex);

		Logger::Get().m_sinks.emplace(
			name,
			std::make_unique<T>(
				std::forward<decltype(args)>(args)...)
		);
	}
}

#define crylog_info(...)    Logger::Get().Log(LogLevel::info,    std::source_location::current(), __VA_ARGS__)
#define crylog_warning(...) Logger::Get().Log(LogLevel::warning, std::source_location::current(), __VA_ARGS__)
#define crylog_error(...)   Logger::Get().Log(LogLevel::error,   std::source_location::current(), __VA_ARGS__)

#define cryfmtlog_info(fmt, ...)    Logger::Get().FormatLog(LogLevel::info,    std::source_location::current(), fmt, __VA_ARGS__)
#define cryfmtlog_warning(fmt, ...) Logger::Get().FormatLog(LogLevel::warning, std::source_location::current(), fmt, __VA_ARGS__)
#define cryfmtlog_error(fmt, ...)   Logger::Get().FormatLog(LogLevel::error,   std::source_location::current(), fmt, __VA_ARGS__)

