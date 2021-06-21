#pragma once
#include <string>
#include "LogLevels.h"

namespace Crystal {
	class ISink {
	public:
		ISink() = default;
		virtual ~ISink() = default;

		virtual void Emit(std::string_view message, LogLevel lvl) noexcept = 0;
	};
}
