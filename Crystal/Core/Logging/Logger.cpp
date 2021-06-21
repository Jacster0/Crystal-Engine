#include "Logger.h"

using namespace Crystal;

Logger& Logger::Get() noexcept {
	static Logger logger;
	return logger;
}

void Logger::SetLevel(LogLevel lvl) {
	m_level = lvl;
}

void Logger::AttachSink(std::unique_ptr<ISink> sink) noexcept {
	m_sink = std::move(sink);
}