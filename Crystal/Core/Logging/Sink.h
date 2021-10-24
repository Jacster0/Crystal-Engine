#pragma once
#include <string>
#include <source_location>
#include "LogLevels.h"

namespace Crystal {
	class ISink {
	public:
		virtual void Emit(std::string_view message, LogLevel lvl, const std::source_location& loc = std::source_location::current()) noexcept = 0;
	};
}
