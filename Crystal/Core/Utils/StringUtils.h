#pragma once
#include <string_view>
#include <cstdlib>

namespace Crystal {

	namespace detail {
		template<class T>
		constexpr bool is_string_v = false;

		template<class T>
		struct is_string : std::false_type {};

		template<class Elem, class Traits, class Alloc>
		struct is_string<std::basic_string<Elem, Traits, Alloc>> : std::true_type {};

		template<class Elem, class Traits, class Alloc>
		constexpr bool is_string_v<std::basic_string<Elem, Traits, Alloc>> = true;

		template<class T>
		using can_construct_string = std::is_constructible<std::string, T>;

		template<class T>
		using can_construct_wstring = std::is_constructible<std::wstring, T>;

		template<class T>
		constexpr bool can_construct_string_v = std::is_constructible_v<std::string, T>;

		template<class T>
		constexpr bool can_construct_wstring_v = std::is_constructible_v<std::wstring, T>;

		std::string inline ToNarrow(std::wstring_view wide) noexcept {
			std::string narrow(wide.size() + 1, ' ');
			wcstombs_s(nullptr, narrow.data(), narrow.size(), wide.data(), static_cast<size_t>(-1));

			return narrow;
		}

		std::wstring inline ToWide(std::string_view narrow) noexcept {
			std::wstring wide(narrow.size() + 1, ' ');
			mbstowcs_s(nullptr, wide.data(), wide.size(), narrow.data(), static_cast<size_t>(-1));

			return wide;
		}
	}

	namespace StringConverter {
		template<class To, class From>
		constexpr auto ConvertTo(From&& from) noexcept ->
		std::enable_if_t <
			std::conjunction_v<
				std::disjunction<detail::is_string<From>, detail::can_construct_string<From>, detail::can_construct_wstring<From>>,
				std::disjunction<detail::is_string<To>, detail::can_construct_string<To>, detail::can_construct_wstring<To>>
			>,
			To
		>
		{
			if constexpr (std::is_same_v<std::string, std::decay_t<To>>) {
				if constexpr (std::is_same_v<std::string, std::decay_t<From>>) {
					return from;
				}
				else if constexpr (detail::can_construct_string_v<From>) {
					return std::string(std::forward<From>(from));
				}
				else {
					return detail::ToNarrow(std::forward<From>(from));
				}
			}

			else if constexpr (std::is_same_v<std::wstring, std::decay_t<To>>) {
				if constexpr (std::is_same_v<std::wstring, std::decay_t<From>>) {
					return from;
				}
				else if constexpr (detail::can_construct_wstring_v<From>) {
					return std::wstring(std::forward<From>(from));
				}
				else {
					return detail::ToWide(std::forward<From>(from));
				}
			}
		}
	}
	
	[[nodiscard]] constexpr bool IsEmptyOrWhiteSpace(std::string_view str) noexcept {
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