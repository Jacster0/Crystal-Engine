#include "CrystalApi.h"
#include "../Platform/Windows/Window.h"
#include "../Core/Application.h"

#include <vector>

using namespace Crystal;

Application* application;

struct {
	int NumLogicalProcessors;
	int NumCores;
	const char* BrandString;
	const char* Vendor;
	const char* Architecture;
} cpuInfo;


CRYSTAL_API void create_render_surface(HWND parent, const uint32_t width, const uint32_t height) noexcept {
	application = new Application(
		ApplicationCreateInfo {
			.ParentHwnd = parent,
			.Width      = width,
			.Height     = height
		}
	);
}

CRYSTAL_API void destroy_render_surface() noexcept {
	delete application;	
}

CRYSTAL_API HWND get_window_handle() noexcept {
	return application->GetWindow().GetWindowHandle();
}

CRYSTAL_API void* get_cpu_information() noexcept {
	auto& info = application->GetCpuInfo();

	cpuInfo.NumLogicalProcessors = info.Info.NumLogicalProcessors;
	cpuInfo.NumCores             = info.Info.NumCores;
	cpuInfo.BrandString          = info.Info.BrandString.c_str();
	cpuInfo.Vendor               = info.Info.Vendor.c_str();
	cpuInfo.Architecture         = info.Info.Architecture.c_str();

	return static_cast<void*>(&cpuInfo);
}
