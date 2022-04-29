#pragma once
#include <array>
#include "../Core/Math/Vector3.h"
#include <d3d12.h>

namespace Crystal {
    struct VertexElement {
		static constexpr auto Position  = "POSITION";
		static constexpr auto Normal    = "NORMAL";
		static constexpr auto Tangent   = "TANGENT";
		static constexpr auto Bitangent = "BITANGENT";
		static constexpr auto Texcoord  = "TEXCOORD";
    };

	struct VertexAlignment {
		static constexpr unsigned append = 0xffffffff;
	};

	struct Vertex {
		Math::Vector3 Position{};
		Math::Vector3 Normal{};
		Math::Vector3 Tangent{};
		Math::Vector3 Bitangent{};
		Math::Vector3 TexCoord{};

		static constexpr D3D12_INPUT_LAYOUT_DESC InputLayout = 
		{
			std::array<D3D12_INPUT_ELEMENT_DESC, 5>
			{
				{
					{ VertexElement::Position,  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, VertexAlignment::append, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ VertexElement::Normal,    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, VertexAlignment::append, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ VertexElement::Tangent,   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, VertexAlignment::append, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ VertexElement::Bitangent, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, VertexAlignment::append, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ VertexElement::Texcoord,  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, VertexAlignment::append, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				}
			}.data(),
			5
		};
	};
}