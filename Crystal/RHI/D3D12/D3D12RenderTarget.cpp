#include "D3D12RenderTarget.h"
#include "D3D12Texture.h"

#include <algorithm>

using namespace Crystal;

void RenderTarget::AttachTexture(AttachmentPoint attachmentPoint, std::shared_ptr<Texture> texture) noexcept {
	m_textures.at(attachmentPoint) = texture;

	if (texture && texture->GetUnderlyingResource()) {
		const auto desc = texture->GetResourceDesc();

		m_size.Width  = static_cast<uint32_t>(desc.Width);
		m_size.Height = static_cast<uint32_t>(desc.Height);
	}
}

std::shared_ptr<Texture> RenderTarget::GetTexture(AttachmentPoint attachmentPoint) const noexcept {
	return m_textures.at(attachmentPoint);
}

void RenderTarget::Resize(USize size) noexcept {
	m_size = size;
	Resize(m_size.Width, m_size.Height);
}

void RenderTarget::Resize(uint32_t width, uint32_t height) noexcept {
	for (auto& texture : m_textures) {
		if (texture) {
			texture->Resize(width, height);
		}
	}
}

Viewport RenderTarget::GetViewPort(FSize scale, FSize bias, float minDepth, float maxDepth) const noexcept {
	uint64_t width  = 0;
	uint32_t height = 0;

	for (int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color7; i++) {
		if (const auto& texture = m_textures.at(i)) {
			const auto desc = texture->GetResourceDesc();

			width  = std::max(width, desc.Width);
			height = std::max(height, desc.Height);
		}
	}

	return {
		static_cast<float>(width  * bias.First),
		static_cast<float>(height * bias.Second),
		static_cast<float>(width  * scale.First),
		static_cast<float>(height * scale.Second),
		minDepth,
		maxDepth
	};
}

const std::vector<std::shared_ptr<Texture>>& RenderTarget::GetTextures() const noexcept {
	return m_textures;
}

D3D12_RT_FORMAT_ARRAY RenderTarget::GetRenderTargetFormats() const noexcept {
	D3D12_RT_FORMAT_ARRAY rtvFormats = {};

	for (int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color7; i++) {
		if (const auto texture = m_textures.at(i)) {
			rtvFormats.RTFormats[rtvFormats.NumRenderTargets++] = texture->GetResourceDesc().Format;
		}
	}

	return rtvFormats;
}

DXGI_FORMAT RenderTarget::GetDepthStencilFormat() const noexcept {
	DXGI_FORMAT dsvFormat = DXGI_FORMAT_UNKNOWN;

	if (const auto depthStencilTexture = m_textures[AttachmentPoint::DepthStencil]) {
		dsvFormat = depthStencilTexture->GetResourceDesc().Format;
	}

	return dsvFormat;
}

DXGI_SAMPLE_DESC RenderTarget::GetSampleDesc() const noexcept {
	DXGI_SAMPLE_DESC sampleDesc = { 1, 0 };
	for (int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color7; i++) {
		if (const auto texure = m_textures.at(i)) {
			sampleDesc = texure->GetResourceDesc().SampleDesc;
			break;
		}
	}

	return sampleDesc;
}
