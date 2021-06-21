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

		void Log(LogLevel lvl, auto&&... args) noexcept;
		void FormatLog(LogLevel lvl, std::string_view fmt, auto&&... args) noexcept;
		void AttachSink(const std::shared_ptr<ISink>& sink) noexcept;
		void RemoveSink(const std::string_view name) noexcept;
	private:
		Logger()                             = default;
		Logger(const Logger& rhs)            = delete;
		Logger& operator=(const Logger& rhs) = delete;
		Logger(Logger&& rhs)                 = delete;
		Logger& operator=(Logger&& rhs)      = delete;
		~Logger()                            = default;
 		
		void Log(auto&&... args) const noexcept;
		void FormatLog(std::string_view fmt, auto&&... args) const noexcept;
		void SetLevel(LogLevel lvl);

		std::mutex m_loggingMutex;
		LogLevel m_level = LogLevel::normal;
		
		std::unordered_map<std::string_view, std::shared_ptr<ISink>> m_sinks;
	};

	inline void Logger::Log(LogLevel lvl, auto&& ...args) noexcept {
		std::scoped_lock lock(m_loggingMutex);
		SetLevel(lvl);
		Log(std::forward<decltype(args)>(args)...);
	}

	inline void Logger::FormatLog(LogLevel lvl, std::string_view fmt, auto&& ...args) noexcept {
		std::scoped_lock lock(m_loggingMutex);
		SetLevel(lvl);
		FormatLog(fmt, std::forward<decltype(args)>(args)...);
	}

	inline void Logger::Log(auto&& ...args) const noexcept {
		std::stringstream ss;
		(ss << ... << args);

		for (const auto& [key, sink] : m_sinks) {
			sink->Emit(ss.str(), m_level);
		}
	}

	inline void Logger::FormatLog(std::string_view fmt, auto&& ...args) const noexcept {
		std::string message = std::format(fmt, std::forward<decltype(args)>(args)...);

		for (const auto& [key, sink] : m_sinks) {
			sink->Emit(message, m_level);
		}
	}
}

namespace Crystal::crylog {
	inline void info(auto&&... args) noexcept {
		Logger::Get().Log(LogLevel::info, std::forward<decltype(args)>(args)..., Logger::NewLine());
	}

	inline void warning(auto&&... args) noexcept {
		Logger::Get().Log(LogLevel::warning, std::forward<decltype(args)>(args)..., Logger::NewLine());
	}

	inline void error(auto&&... args) noexcept {
		Logger::Get().Log(LogLevel::error, std::forward<decltype(args)>(args)..., Logger::NewLine());
	}

	inline void critical(auto&&... args) noexcept {
		Logger::Get().Log(LogLevel::critical, std::forward<decltype(args)>(args)..., Logger::NewLine());
	}
}

namespace Crystal::cryfmtlog {
	inline void info(std::string_view fmt, auto&&... args) noexcept {
		Logger::Get().FormatLog(LogLevel::info, fmt, std::forward<decltype(args)>(args)...);
	}

	inline void warning(std::string_view fmt, auto&&... args) noexcept {
		Logger::Get().FormatLog(LogLevel::warning, fmt, std::forward<decltype(args)>(args)...);
	}

	inline void error(std::string_view fmt, auto&&... args) noexcept {
		Logger::Get().FormatLog(LogLevel::error, fmt, std::forward<decltype(args)>(args)...);
	}

	inline void critical(std::string_view fmt, auto&&... args) noexcept {
		Logger::Get().FormatLog(LogLevel::critical, fmt, std::forward<decltype(args)>(args)...);
	}
}