#pragma once
#include <cstdint>
#include "CrystalWindow.h"

namespace Crystal {
	struct ApplicationCreateInfo {
		HWND HWnd;
		HWND ParentHwnd;
		RECT WindowRect;
		uint32_t Width{ 1300u };
		uint32_t Height{ 800u };
	};
}
