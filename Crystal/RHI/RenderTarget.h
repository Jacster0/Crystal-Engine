#pragma once

#define API_D3D12 1

#if API_D3D12
#include "D3D12/D3D12RenderTarget.h"
#elif API_VULKAN
#include "Vulkan/VKRenderTarget.h" //Framebuffer?
#endif
