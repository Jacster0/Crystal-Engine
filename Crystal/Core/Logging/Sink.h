#pragma once
#include <string>
#include "LogLevels.h"

namespace Crystal {
	class ISink {
	public:
		explicit ISink(const std::string& name) noexcept;
		virtual ~ISink() = default;

		[[nodiscard]] const std::string& GetName() const noexcept { return m_name; }
		virtual void Emit(std::string_view message, LogLevel lvl) noexcept = 0;
	protected:
		std::string m_name;
	};
}
