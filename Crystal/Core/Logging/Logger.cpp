#include "Logger.h"

using namespace Crystal;

Logger& Logger::Get() noexcept {
	static Logger logger;
	return logger;
}

void Logger::SetLevel(LogLevel lvl) {
	m_level = lvl;
}

void Crystal::Logger::SetSourceLoc(const std::source_location& loc) noexcept {
	m_sourceLoc = loc;
}

void Logger::AttachSink(const std::shared_ptr<ISinkBase>& sink) noexcept {
	m_sinks.emplace(sink->GetName(), sink);
}

void Crystal::Logger::RemoveSink(const std::string_view name) noexcept {
	m_sinks.erase(name);
}
