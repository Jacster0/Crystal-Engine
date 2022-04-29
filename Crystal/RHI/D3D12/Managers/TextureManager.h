#pragma once

#include "RHI/D3D12/D3D12Texture.h"

namespace DirectX {
	class TexMetadata;
	class ScratchImage;
}
namespace  Crystal {
	class CommandContext;
	namespace TextureManager {
		std::unique_ptr<Texture> LoadTextureFromFile(CommandContext& ctx, std::string_view fileName, bool sRBG);
	};
}
