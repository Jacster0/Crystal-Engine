#pragma once
#include <cstdint>
#include <variant>

namespace Crystal {
	template<typename T>
	requires std::integral<T> || std::floating_point<T>
	struct Size {
		union {
			T Width;
			T First;
		};
		
		union {
			T Height;
			T Second;
		};
	};

	using USize = Size<uint32_t>;
	using FSize = Size<float>;
}