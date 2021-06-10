#pragma once
#include <source_location>
#include <exception>
#include <format>

#include "../Platform/Windows/CrystalWindow.h"
#include "../Core/Exceptions/CrystalException.h"

namespace Crystal {
	class D3D12Exception final : public CrystalException {
	public:
		D3D12Exception(HRESULT hr, const std::string& functionName, const std::string& file, int line) noexcept;
		[[nodiscard]] const char* what() const noexcept override;
		[[nodiscard]] const std::string GetType() const noexcept override;
		[[nodiscard]] const std::string GetOriginString() const noexcept override;
	private:
		HRESULT m_errorCode = S_OK;
		std::string m_functionName;
	};

	inline void ThrowIfFailed(HRESULT hr, const std::source_location& loc = std::source_location::current()) {
		if (FAILED(hr)) {
			throw D3D12Exception(hr, loc.function_name(), loc.file_name(), loc.line());
		}
	}
}