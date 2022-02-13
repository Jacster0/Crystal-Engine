#pragma once
#include <chrono>
#include <string>
#include "../Lib/FixedString.h"
#include <format>

namespace Crystal {
    namespace detail {
         std::string date_time_format_helper(std::string_view fmt) noexcept {
             const std::chrono::zoned_time local = { std::chrono::current_zone(), std::chrono::system_clock::now() };

             if(fmt.find("%R") != std::string::npos) {
                 const auto seconds = std::stoi(std::format("{:%S}", local));

                 if (seconds < 10) {
                     return std::format(std::format("{}{}", fmt, ":{}"), local, std::string("0").append(std::to_string(seconds)));
                }
                 return std::format(std::format("{}{}", fmt, ":{}"), local, seconds);
            }
             return std::format(fmt, local);
        }
    }

    [[nodiscard]] inline std::string current_date()      noexcept { return detail::date_time_format_helper("{:%F}"); }
    [[nodiscard]] inline std::string current_time()      noexcept { return detail::date_time_format_helper("{:%R}"); }
    [[nodiscard]] inline std::string current_date_time() noexcept { return detail::date_time_format_helper("{:%F %R}"); }
}