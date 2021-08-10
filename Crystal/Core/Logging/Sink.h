#pragma once
#include <string>
#include <source_location>
#include "LogLevels.h"

namespace Crystal {
	class ISinkBase {
	public:
		virtual ~ISinkBase() = default;

		[[nodiscard]] const std::string& GetName() const noexcept { return m_name; }
		virtual void Emit(std::string_view message, LogLevel lvl, const std::source_location& loc = std::source_location::current()) noexcept = 0;
	protected:
		std::string m_name;
	};

	template<typename T>
	class ISink : public ISinkBase {
	public:
		virtual void Emit(
			std::string_view message, 
			LogLevel lvl, 
			const std::source_location& loc = std::source_location::current()) noexcept override 
		{
			static_cast<T*>(this)->Emit(message, lvl, loc);
		}
	};
}
