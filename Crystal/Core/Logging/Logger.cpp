#include "Logger.h"

using namespace Crystal;

std::mutex Logger::m_sinkMutex;

Logger& Logger::Get() noexcept {
	static Logger logger;
	return logger;
}

void Logger::AttachSink(const std::shared_ptr<ISinkBase>& sink) noexcept {
	std::scoped_lock lock(m_sinkMutex);

	auto& logger = Logger::Get();
	logger.m_sinks.emplace(sink->GetName(), sink);
}

void Logger::RemoveSink(std::string_view name) noexcept {
	std::scoped_lock lock(m_sinkMutex);

	auto& logger = Logger::Get();
	logger.m_sinks.erase(name);
}
