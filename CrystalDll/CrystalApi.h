#pragma once
#include "../Platform/Windows/CrystalWindow.h"
#include <cstdint>


#ifdef __cplusplus
#ifndef CRYSTAL_API
#define CRYSTAL_API extern "C" __declspec(dllexport)
#endif
#else
#ifndef CRYSTAL_API
#define CRYSTAL_API __declspec(dllexport)
#endif
#endif

CRYSTAL_API void create_render_surface(HWND parent, const uint32_t width, const uint32_t height) noexcept;
CRYSTAL_API void destroy_render_surface() noexcept;
CRYSTAL_API HWND get_window_handle() noexcept;
CRYSTAL_API void* get_cpu_information() noexcept;