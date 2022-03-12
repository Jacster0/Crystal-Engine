#include "Mesh.h"
#include "../RHI/Buffer.h"
#include "../RHI/CommandContext.h"

using namespace Crystal;

void Mesh::SetPrimitiveTopology(PrimitiveTopology topology) noexcept {
	m_topology = topology;
}

void Mesh::SetVertexBuffer(uint32_t slotID, std::unique_ptr<Buffer>&& vertexBuffer) noexcept {
	m_vertexBuffers[slotID] = std::move(vertexBuffer);
}

void Mesh::SetIndexBuffer(std::unique_ptr<Buffer>&& indexBuffer) noexcept {
	m_indexBuffer = std::move(indexBuffer);
}

void Mesh::Render(GraphicsContext& ctx, uint32_t instanceCount, uint32_t firstInstance) {
	if (m_indexBuffer) [[likely]] {
		ctx.SetPrimitiveTopology(m_topology);

		for (const auto& [slotID, vertexBuffer] : m_vertexBuffers) {
			ctx.SetVertexBuffer(slotID, vertexBuffer.get());
		}

		const auto indexCount  = m_indexBuffer->Count();
		const auto vertexCount = m_vertexBuffers.at(0)->Count();

		if (indexCount > 0) {
			ctx.SetIndexBuffer(m_indexBuffer.get());
			ctx.DrawIndexed(indexCount, instanceCount, 0u, 0u, firstInstance);
		}
		else if (vertexCount > 0) {
			ctx.Draw(vertexCount, instanceCount, 0u, firstInstance);
		}
	}
}
