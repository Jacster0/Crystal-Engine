#include "CrystalApi.h"
#include "../Platform/Windows/Window.h"
#include "../Core/Application.h"

#include <vector>

using namespace Crystal;

std::vector<Application*> applications;

CRYSTAL_API void create_render_surface(HWND parent, const uint32_t width, const uint32_t height)  noexcept {
	Application* app = new Application(
		ApplicationCreateInfo {
			.ParentHwnd = parent,
			.Width      = width,
			.Height     = height
		}
	);

	applications.push_back(app);
}

CRYSTAL_API void destroy_render_surface() noexcept {
	delete applications.front();
	applications.erase(applications.begin());	
}

CRYSTAL_API HWND get_window_handle() noexcept {
	return applications.front()->GetWindow().GetWindowHandle();
}
