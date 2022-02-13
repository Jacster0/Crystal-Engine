#include "CrystalException.h"
#include <format>

using namespace Crystal;

CrystalException::CrystalException(int line, const std::string& file) noexcept
	:
	m_line(line),
	m_file(file)
{}

const char* CrystalException::what() const noexcept {
	m_message = std::format("{}\n{}", GetType(), GetOriginString());
	return m_message.c_str();
}

uint32_t CrystalException::GetLine() const noexcept {
	return m_line;
}

const std::string& CrystalException::GetFile() const noexcept {
	return m_file;
}

std::string CrystalException::GetOriginString() const noexcept {
	return std::format("File: {}\n Line: {}", m_file, m_line);
}

std::string CrystalException::GetType() const noexcept {
	return "Crystal Exception";
}