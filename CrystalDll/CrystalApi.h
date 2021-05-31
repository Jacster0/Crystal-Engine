#pragma once
#include "../Core/Application.h"

#ifndef CRYSTAL_API
#define CRYSTAL_API extern "C" __declspec(dllexport)
#endif

using namespace Crystal;

CRYSTAL_API void  create_render_surface();