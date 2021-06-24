#include "NamedPipeClient.h"

using namespace Crystal;

NamedPipeClient::NamedPipeClient(const std::string& name) noexcept 
	:
	m_name(m_format + name),
	m_hPipe(nullptr)
{}

void NamedPipeClient::Open() noexcept {
	m_hPipe = CreateFileA(
		m_name.c_str(),
		static_cast<DWORD>(m_access),
		static_cast<DWORD>(m_shareMode),
		nullptr,
		static_cast<DWORD>(m_creationDisposition),
		0,
		nullptr);
}

void NamedPipeClient::Close() const noexcept {
	CloseHandle(m_hPipe);
}

void NamedPipeClient::Send(const void* buffer, size_t numBytesToWrite) const noexcept {
	WriteFile(m_hPipe, buffer, numBytesToWrite, nullptr, nullptr);
}

