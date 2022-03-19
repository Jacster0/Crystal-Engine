#pragma once
#include <string_view>

namespace Crystal {
	namespace Log_tag {
		constexpr auto Default  = "Crystal";
		constexpr auto Compute  = "CrystalCompute";
		constexpr auto Core     = "CrystalCore: ";
		constexpr auto Extern   = "CrystalExtern: ";
		constexpr auto Gfx      = "CrystalGfx: ";
		constexpr auto Net      = "CrystalNet: ";
		constexpr auto Platform = "CrystalPlatform: ";
		constexpr auto RHI      = "CrystalRHI: ";
	}

	namespace Log_utils {
		[[nodiscard]] constexpr auto parse_log_tag(std::string_view path) noexcept {
			if (path.contains(R"(CrystalEngine\Crystal\Crystal\Compute))")) {
				return Log_tag::Compute;
			}
			if (path.contains(R"(CrystalEngine\Crystal\Crystal\Core)")) {
				return Log_tag::Core;
			}
			if (path.contains(R"(CrystalEngine\Crystal\Crystal\Extern))")) {
				return Log_tag::Extern;
			}
			if (path.contains(R"(CrystalEngine\Crystal\Crystal\Graphics)")) {
				return Log_tag::Gfx;
			}
			if (path.contains(R"(CrystalEngine\Crystal\Crystal\NetWorking))")) {
				return Log_tag::Net;
			}
			if (path.contains(R"(CrystalEngine\Crystal\Crystal\Platform)")) {
				return Log_tag::Platform;
			}
			if (path.contains(R"(CrystalEngine\Crystal\Crystal\RHI)")) {
				return Log_tag::RHI;
			}
			return Log_tag::Default;
		}
	}
}

