#pragma once

#include <exception>
#include <string>

namespace Crystal {
	class CrystalException : public std::exception {
	 public:
		CrystalException(int line, const std::string& file) noexcept;
		virtual ~CrystalException();

		[[nodiscard]] virtual const char* what() const noexcept override;
		[[nodiscard]] const uint32_t GetLine() const noexcept;
		[[nodiscard]] const std::string& GetFile() const noexcept;
		[[nodiscard]] virtual const std::string GetOriginString() const noexcept;
		[[nodiscard]] virtual const std::string GetType() const noexcept;
	 protected:
		 mutable std::string m_message;
	 private:
		 uint32_t m_line{};
		 std::string m_file;
	};
}