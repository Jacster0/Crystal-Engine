#pragma once
#include <iostream>
#include <mutex>
#include <string>
#include <format>
#include <memory>
#include <unordered_map>

#include "Sink.h"
#include "LogLevels.h"

namespace Crystal {
	class Logger {
	public:
		[[nodiscard]] static Logger& Get() noexcept;
		[[nodiscard]] static constexpr auto NewLine() noexcept { return std::endl<char, std::char_traits<char>>; }

		void Log(LogLevel lvl, const std::source_location& loc, auto&&... args) noexcept;
		void FormatLog(LogLevel lvl, std::string_view fmt, const std::source_location& loc, auto&&... args) noexcept;
		void AttachSink(const std::shared_ptr<ISink>& sink) noexcept;
		void RemoveSink(const std::string_view name) noexcept;
	private:
		Logger()                             = default;
		Logger(const Logger& rhs)            = delete;
		Logger& operator=(const Logger& rhs) = delete;
		Logger(Logger&& rhs)                 = delete;
		Logger& operator=(Logger&& rhs)      = delete;
		~Logger()                            = default;
 		
		void InternalLog(auto&&... args) const noexcept;
		void InternalFormatLog(std::string_view fmt, auto&&... args) const noexcept;
		void SetLevel(LogLevel lvl);
		void SetSourceLoc(const std::source_location& loc) noexcept;

		std::mutex m_loggingMutex;
		LogLevel m_level = LogLevel::info;
		std::source_location m_sourceLoc;
		
		std::unordered_map<std::string_view, std::shared_ptr<ISink>> m_sinks;
	};

	inline void Logger::Log(LogLevel lvl, const std::source_location& loc, auto && ...args) noexcept {
		std::scoped_lock lock(m_loggingMutex);
		SetLevel(lvl);
		SetSourceLoc(loc);
		InternalLog(std::forward<decltype(args)>(args)...);
	}

	inline void Logger::InternalLog(auto&& ...args) const noexcept {
		std::stringstream ss;
		(ss << ... << args);

		const std::string message = ss.str();

		for (const auto& [key, sink] : m_sinks) {
			sink->Emit(message, m_level, m_sourceLoc);
		}
	}

	inline void Logger::FormatLog(LogLevel lvl, std::string_view fmt, const std::source_location& loc, auto&& ...args) noexcept {
		std::scoped_lock lock(m_loggingMutex);
		SetLevel(lvl);
		SetSourceLoc(loc);
		InternalFormatLog(fmt, std::forward<decltype(args)>(args)...);
	}

	inline void Logger::InternalFormatLog(std::string_view fmt, auto&& ...args) const noexcept {
		std::string message = std::format(fmt, std::forward<decltype(args)>(args)...);

		for (const auto& [key, sink] : m_sinks) {
			sink->Emit(message, m_level, m_sourceLoc);
		}
	}
}

#define crylog_info(...)    Logger::Get().Log(LogLevel::info, std::source_location::current(), __VA_ARGS__)
#define crylog_warning(...) Logger::Get().Log(LogLevel::warning, std::source_location::current(), __VA_ARGS__)
#define crylog_error(...)   Logger::Get().Log(LogLevel::error, std::source_location::current(), __VA_ARGS__)

#define cryfmtlog_info(fmt, ...)    Logger::Get().FormatLog(LogLevel::info, fmt, std::source_location::current(), __VA_ARGS__)
#define cryfmtlog_warning(fmt, ...) Logger::Get().FormatLog(LogLevel::warning, fmt, std::source_location::current(), __VA_ARGS__)
#define cryfmtlog_error(fmt, ...)   Logger::Get().FormatLog(LogLevel::error, fmt, std::source_location::current(), __VA_ARGS__)
