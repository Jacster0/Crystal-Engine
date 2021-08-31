#pragma once

#define API_D3D12 1

#if API_D3D12
#include "D3D12/D3D12Texture.h"
#elif API_VULKAN
#include "Vulkan/VKTexture.h"
#endif
