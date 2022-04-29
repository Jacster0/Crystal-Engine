#include "CrystalApi.h"
#include "Platform/Windows/Window.h"
#include "Core/Application.h"
#include "Platform/Windows/MessageBox.h"
#include "Core/Exceptions/CrystalException.h"
#include <vector>

using namespace Crystal;

Application* application;
std::thread t1;

struct {
	int NumLogicalProcessors;
	int NumCores;
	const char* BrandString;
	const char* Vendor;
	const char* Architecture;
} cpuInfo;


CRYSTAL_API void create_render_surface(HWND parent, const uint32_t width, const uint32_t height) noexcept {
	try
	{
		application = new Application(
			ApplicationCreateInfo{
				.ParentHwnd = parent,
				.Width = width,
				.Height = height
			}
		);

		t1 = std::thread(&Application::Run, application);
	}
	catch (const CrystalException& e) {
		MessageBox::Show(e.what(), e.GetType(), MessageBox::Buttons::OK, MessageBox::Icon::Exclamation);
	}
	catch (const std::exception& e) {
		MessageBox::Show(e.what(), "Standard exception", MessageBox::Buttons::OK, MessageBox::Icon::Exclamation);
	}
	catch (...) {
		MessageBox::Show("No details available", "unknown exception", MessageBox::Buttons::OK, MessageBox::Icon::Exclamation);
	}
}

CRYSTAL_API void destroy_render_surface() noexcept {
	t1.join();
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

	return &cpuInfo;
}

CRYSTAL_API void message_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	//application->GetWindow().MsgProc(hWnd, msg, wParam, lParam);
}
