#pragma once
#include <type_traits>

namespace Crystal::crylib {
	template<class T, class U, class = void>
	struct HasType : std::false_type {};

	template<class T, class U>
	struct HasType<T, U, std::void_t<typename T::U>> : std::true_type {};

	template<class T, class U>
	inline constexpr bool HasType_v = HasType<T, U>::value;
}