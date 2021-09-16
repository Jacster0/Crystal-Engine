#pragma once
#include <chrono>
#include <string>
#include "../Lib/FixedString.h"

namespace Crystal {
	namespace detail {
		template<crylib::FixedString str>
		inline constexpr std::string date_time_format_helper() noexcept {
			constexpr std::string_view fmt(str);
			const std::chrono::zoned_time local = { std::chrono::current_zone(), std::chrono::system_clock::now() };

			if constexpr (fmt.find("%R") != std::string::npos) {
				return std::format(std::format("{}{}", fmt, ":{}"), local, std::stoi(std::format("{:%S}", local)));
			}
			else {
				return std::format(fmt, local);
			}
		}
	}

	[[nodiscard]] inline std::string current_date()      noexcept { return detail::date_time_format_helper<"{:%F}">(); }
	[[nodiscard]] inline std::string current_time()      noexcept { return detail::date_time_format_helper<"{:%R}">(); }
	[[nodiscard]] inline std::string current_date_time() noexcept { return detail::date_time_format_helper<"{:%F %R}">(); }
}