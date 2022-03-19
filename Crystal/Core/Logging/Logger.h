#pragma once
#include <mutex>
#include <string>
#include <format>
#include <memory>
#include <source_location>
#include <concepts>

#include "Core/Utils/LogUtils.h"
#include "LogLevels.h"
#include "Sink.h"

namespace Crystal {
	namespace detail{
		struct log_fmt {
			template<class T>
			log_fmt(T&& msg, std::source_location loc = std::source_location::current())
		        :
		        msg{ std::forward<T>(msg) },
		        loc{ loc }
		    {}

		    std::string_view msg;
		    std::source_location loc;
		};
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
			std::scoped_lock lock(Get().m_sinkMutex);
			Get().m_sinks.emplace_back(std::make_unique<T>(std::forward<decltype(args)>(args)...));
		}

		static void AddSink(std::unique_ptr<ISink>&& sink) noexcept {
			std::scoped_lock lock(Get().m_sinkMutex);
			Get().m_sinks.emplace_back(std::move(sink));
		}

		template<std::derived_from<ISink> T>
		static void RemoveSink() noexcept {
			std::scoped_lock lock(Get().m_sinkMutex);
			std::erase_if(Get().m_sinks, [](const auto& sink) { return typeid(*sink) == typeid(T); });
		}

		constexpr void Log(std::string_view tag, LogLevel lvl, const detail::log_fmt& fmt, auto&&... args) const noexcept {
		    std::scoped_lock lock(m_loggingMutex);

		    const std::string message = std::string(tag).append(std::format(fmt.msg, std::forward<decltype(args)>(args)...));

		    for (const auto& sink : m_sinks) {
		        sink->Emit(message, lvl, fmt.loc);
		    }
		}
	private:
		Logger()  = default;
		~Logger() = default;

		mutable std::mutex m_loggingMutex;
		std::mutex m_sinkMutex;
		std::vector<std::unique_ptr<ISink>> m_sinks;
	};
}

constexpr void crylog_info(Crystal::detail::log_fmt fmt, auto&& args...) noexcept {
	const auto tag = Crystal::log_utils::parse_log_tag(fmt.loc.file_name());
	Crystal::Logger::Get().Log(tag, Crystal::LogLevel::info, fmt, std::forward<decltype(args)>(args));
}

constexpr void crylog_info(std::string_view msg, std::source_location loc = std::source_location::current()) noexcept {
	const auto tag = Crystal::log_utils::parse_log_tag(loc.file_name());
	Crystal::Logger::Get().Log(tag, Crystal::LogLevel::info, { "{}", loc }, msg);
}

constexpr void crylog_warning(Crystal::detail::log_fmt fmt, auto&& args...) noexcept {
	const auto tag = Crystal::log_utils::parse_log_tag(fmt.loc.file_name());
	Crystal::Logger::Get().Log(tag, Crystal::LogLevel::warning, fmt, std::forward<decltype(args)>(args));
}

constexpr void crylog_warning(std::string_view msg, std::source_location loc = std::source_location::current()) noexcept {
	const auto tag = Crystal::log_utils::parse_log_tag(loc.file_name());
	Crystal::Logger::Get().Log(tag, Crystal::LogLevel::warning, { "{}", loc }, msg);
}

constexpr void crylog_error(Crystal::detail::log_fmt fmt, auto&& args...)  noexcept {
	const auto tag = Crystal::log_utils::parse_log_tag(fmt.loc.file_name());
	Crystal::Logger::Get().Log(tag, Crystal::LogLevel::error, fmt, std::forward<decltype(args)>(args));
}

constexpr void crylog_error(std::string_view msg, std::source_location loc = std::source_location::current()) noexcept {
	const auto tag = Crystal::log_utils::parse_log_tag(loc.file_name());
	Crystal::Logger::Get().Log(tag, Crystal::LogLevel::error, { "{}", loc }, msg);
}

constexpr void crylog_debug(Crystal::detail::log_fmt fmt, auto&& args...)  noexcept {
#if _DEBUG
	const auto tag = Crystal::log_utils::parse_log_tag(fmt.loc.file_name());
	Crystal::Logger::Get().Log(tag, Crystal::LogLevel::debug, fmt, std::forward<decltype(args)>(args));
#endif
}

constexpr void crylog_debug(std::string_view msg, std::source_location loc = std::source_location::current()) noexcept {
#if _DEBUG
	const auto tag = Crystal::log_utils::parse_log_tag(loc.file_name());
	Crystal::Logger::Get().Log(tag, Crystal::LogLevel::debug, { "{}", loc }, msg);
#endif
}