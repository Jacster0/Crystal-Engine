#pragma once
#include <string_view>

namespace Crystal {
	namespace log_tag {
		constexpr auto Default  = "Crystal";
		constexpr auto Compute  = "CrystalCompute";
		constexpr auto Core     = "CrystalCore: ";
		constexpr auto Extern   = "CrystalExtern: ";
		constexpr auto Gfx      = "CrystalGfx: ";
		constexpr auto Net      = "CrystalNet: ";
		constexpr auto Platform = "CrystalPlatform: ";
		constexpr auto RHI      = "CrystalRHI: ";
	}

	namespace log_utils {
		[[nodiscard]] constexpr auto parse_log_tag(std::string_view path) noexcept {
			if (path.contains(R"(CrystalEngine\Crystal\Crystal\Compute))")) {
				return log_tag::Compute;
			}
			if (path.contains(R"(CrystalEngine\Crystal\Crystal\Core)")) {
				return log_tag::Core;
			}
			if (path.contains(R"(CrystalEngine\Crystal\Crystal\Extern))")) {
				return log_tag::Extern;
			}
			if (path.contains(R"(CrystalEngine\Crystal\Crystal\Graphics)")) {
				return log_tag::Gfx;
			}
			if (path.contains(R"(CrystalEngine\Crystal\Crystal\NetWorking))")) {
				return log_tag::Net;
			}
			if (path.contains(R"(CrystalEngine\Crystal\Crystal\Platform)")) {
				return log_tag::Platform;
			}
			if (path.contains(R"(CrystalEngine\Crystal\Crystal\RHI)")) {
				return log_tag::RHI;
			}
			return log_tag::Default;
		}
	}
}

