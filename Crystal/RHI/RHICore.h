#pragma once

#define API_D3D12 1

#if API_D3D12
#include "D3D12/D3D12Core.h"
#include "D3D12/D3D12Buffer.h"
#include "D3D12/D3D12RootSignature.h"
#include "D3D12/D3D12SwapChain.h"
#include "D3D12/D3D12CommandContext.h"
#include "D3D12/D3D12RenderTarget.h"
#include "D3D12/D3D12Texture.h"
#elif API_VULKAN
#include "Vulkan/VKCore.h"
#endif


