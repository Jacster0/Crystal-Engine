#pragma once

namespace Crystal::crylib {
	template<size_t Length>
	struct FixedString {
		constexpr FixedString(const char* const str) noexcept {
			std::copy_n(str, Length, Value);
		}

		[[nodiscard]] constexpr explicit operator const char* () const noexcept { return Value; }

		[[nodiscard]] constexpr char* begin() const noexcept { return Value; }
		[[nodiscard]] constexpr char* end() const noexcept   { return &Value[Length]; }
		char Value[Length + 1]{};
	};

	template<size_t Length>
	FixedString(const char(&)[Length])->FixedString<Length - 1>;
}
