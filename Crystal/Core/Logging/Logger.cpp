#include "Logger.h"

using namespace Crystal;

Logger& Logger::Get() noexcept {
	static Logger logger;
	return logger;
}

void Logger::SetLevel(LogLevel lvl) {
	m_level = lvl;
}

void Logger::AttachSink(const std::shared_ptr<ISink>& sink) noexcept {
	m_sinks.emplace(sink->GetName(), sink);
}

void Crystal::Logger::RemoveSink(const std::string_view name) noexcept {
	m_sinks.erase(name);
}
