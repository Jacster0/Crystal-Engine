#pragma once
#include <map>
#include <memory>
#include <cstdint>
#include "Types/Types.h"

namespace Crystal {
	class Buffer;
	class GraphicsContext;
	class Mesh {
	public:
		void SetPrimitiveTopology(PrimitiveTopology topology) noexcept;
		void SetVertexBuffer(uint32_t slotID, std::unique_ptr<Buffer>&& vertexBuffer) noexcept;
		void SetIndexBuffer(std::unique_ptr<Buffer>&& indexBuffer) noexcept;

		void Render(GraphicsContext& ctx, uint32_t instanceCount = 1, uint32_t firstInstance = 0);
	private:
		std::map<uint32_t, std::unique_ptr<Buffer>> m_vertexBuffers;
		std::unique_ptr<Buffer> m_indexBuffer{nullptr};

		PrimitiveTopology m_topology{ Topology_t::trianglelist };
	};
}
