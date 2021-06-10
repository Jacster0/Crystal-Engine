#include <string_view>
#include <iomanip>

namespace Crystal {
	static constexpr auto MAX_STR_CONVERT_SIZE = 512u;

	[[nodiscard]] const inline std::string ToNarrow(std::wstring_view wide) {
		char narrow[MAX_STR_CONVERT_SIZE];
		wcstombs_s(nullptr, narrow, wide.data(), _TRUNCATE);

		return std::string(narrow);
	}
	
	[[nodiscard]] const inline std::wstring ToWide(std::string_view narrow) {
		wchar_t wide[MAX_STR_CONVERT_SIZE];
		mbstowcs_s(nullptr, wide, narrow.data(), _TRUNCATE);

		return std::wstring(wide);
	}
}