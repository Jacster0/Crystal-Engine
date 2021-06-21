#include "ManagedLoggerSink.h"
#include <algorithm>

using namespace Crystal;

ManagedLoggerSink::ManagedLoggerSink() noexcept
	:
	m_namedPipe("ManagedLogger"),
	m_messageInfo{}
{}

void ManagedLoggerSink::Emit(std::string_view message, LogLevel lvl) noexcept {
	m_messageInfo = MessageInfo{ .Level = lvl };
	std::ranges::copy(message, m_messageInfo.Message);
	
	m_namedPipe.Open();
	m_namedPipe.Send(static_cast<const void*>(&m_messageInfo), sizeof(m_messageInfo));
	m_namedPipe.Close();
}