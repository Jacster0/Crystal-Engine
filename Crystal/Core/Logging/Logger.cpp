#include "Logger.h"

using namespace Crystal;

std::mutex Logger::m_sinkMutex;

Logger& Logger::Get() noexcept {
	static Logger logger;
	return logger;
}

void Logger::AddSink(std::unique_ptr<ISinkBase>&& sink, std::string_view name) noexcept {
	std::scoped_lock lock(m_sinkMutex);

	Logger::Get().m_sinks.emplace(name, std::move(sink));
}

void Logger::RemoveSink(std::string_view name) noexcept {
	std::scoped_lock lock(m_sinkMutex);

	Logger::Get().m_sinks.erase(name);
}
