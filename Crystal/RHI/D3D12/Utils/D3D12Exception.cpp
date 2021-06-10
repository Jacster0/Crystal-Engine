#include "D3D12Exception.h"
#include "../Core/Utils/StringUtils.h"
#include <comdef.h>

using namespace Crystal;

D3D12Exception::D3D12Exception(HRESULT hr, const std::string& functionName, const std::string& file, int line) noexcept
	:
	m_errorCode(hr),
	m_functionName(functionName),
	CrystalException::CrystalException(line, file)
{}

const char* Crystal::D3D12Exception::what() const noexcept {
	m_message = std::format("{}\n{}",
		GetType(),
		GetOriginString());

	return m_message.c_str();
}

const std::string Crystal::D3D12Exception::GetType() const noexcept {
	return "D3D12 Exception";
}

const std::string Crystal::D3D12Exception::GetOriginString() const noexcept {
	_com_error error(m_errorCode);

	return std::format("File: {}\nFunction: {}\nLine: {}\nError message : {}",
		GetFile(),
		m_functionName,
		GetLine(),
		Crystal::ToNarrow(error.ErrorMessage()));
}
