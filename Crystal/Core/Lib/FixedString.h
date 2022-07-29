#pragma once

namespace Crystal::crylib {
	template<size_t Length>
	struct FixedString {
		constexpr FixedString(const char(&string)[Length]) noexcept {
			std::copy_n(string, Length, Value);
		}

		[[nodiscard]] constexpr explicit operator std::string_view() const noexcept { return Value; }

		[[nodiscard]] constexpr const char* begin() const noexcept { return Value; }
		[[nodiscard]] constexpr const char* end() const noexcept { return &Value[Length]; }

		char Value[Length]{};
	};
}
