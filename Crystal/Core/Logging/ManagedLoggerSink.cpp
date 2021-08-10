#include "ManagedLoggerSink.h"
#include <algorithm>

using namespace Crystal;

ManagedLoggerSink::ManagedLoggerSink() noexcept
	:
	m_namedPipe("ManagedLogger"),
	m_messageInfo{}
{}

void ManagedLoggerSink::Emit(std::string_view message, LogLevel lvl, const std::source_location& loc) noexcept {
	std::string_view fileNameView     = loc.file_name();
	std::string_view functionNameView = loc.function_name();
	
	m_messageInfo      = MessageInfo{ .Level = lvl };
	m_messageInfo.Line = loc.line();

	std::ranges::copy(message, m_messageInfo.Message);
	std::ranges::copy(fileNameView, m_messageInfo.FileName);
	std::ranges::copy(functionNameView, m_messageInfo.FunctionName);
	
	m_namedPipe.Call(m_messageInfo);
}