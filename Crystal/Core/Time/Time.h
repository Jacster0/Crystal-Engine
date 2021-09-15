#pragma once
#include <chrono>

namespace Crystal {
	[[nodiscard]] inline std::string GetCurrrentDate() noexcept {
		const auto local = std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() };

		return std::format("{:%x}", local);
	}

	[[nodiscard]] inline std::string GetCurrrentTime() noexcept {
		const auto local   = std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() };
		const auto seconds = std::stoi(std::format("{:%S}", local));

		return std::format("{:%R}:{}", local, seconds);
	}

	[[nodiscard]] inline std::string GetCurrrentDateTime() noexcept {
		const auto local   = std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() };
		const auto seconds = std::stoi(std::format("{:%S}", local));

		return std::format("{:%x  %R}:{}", local, seconds);
	}
}