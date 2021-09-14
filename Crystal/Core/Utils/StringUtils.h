#pragma once

#include "../Platform/Windows/CrystalWindow.h"
#include <string_view>

namespace Crystal {

	[[nodiscard]] const inline std::string ToNarrow(std::wstring_view wide) {
		const int inLength  = static_cast<int>(wide.length());
		const int outLength = WideCharToMultiByte(CP_UTF8, 0, wide.data(), inLength, nullptr, 0, nullptr, nullptr);

		std::string narrow(outLength, '\0');
		WideCharToMultiByte(CP_UTF8, 0, wide.data(), inLength, narrow.data(), outLength, nullptr, nullptr);

		return narrow;
	}
	
	[[nodiscard]] const inline std::wstring ToWide(std::string_view narrow) {
		const int inLength  = static_cast<int>(narrow.length());
		const int outLength = MultiByteToWideChar(CP_UTF8, 0, narrow.data(), inLength, nullptr, 0);

		std::wstring wide(outLength, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, narrow.data(), inLength, wide.data(), outLength);

		return wide;
	}

	[[nodiscard]] constexpr inline bool IsEmptyOrWhiteSpace(std::string_view str) noexcept {
		//Check if the string is empty
		if (str.empty()) {
			return true;
		}
		//Check if the string contains any non whitespace character
		if (str.find_first_not_of(' ') != std::string::npos) {
			return false;
		}
		return true;
	}
}