#include "D3D12Buffer.h"
#include "D3D12Core.h"
#include "D3D12Texture.h"
#include "Utils/ResourceStateTracker.h"
#include "Utils/D3D12Exception.h"
#include "Utils/d3dx12.h"

using namespace Crystal;
using namespace Microsoft::WRL;

Buffer::Buffer(const BufferDescription& desc) 
	:
	m_bufferDesc(desc)
{
	auto& d3d12Device       = RHICore::get_device();
	const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(Size());
	const auto heapProps    = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	ThrowIfFailed(d3d12Device.CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&m_resource)));

	ResourceStateTracker::AddGlobalResourceState(m_resource.Get(), D3D12_RESOURCE_STATE_COMMON);
}

Buffer::Buffer(const BufferDescription& desc, const Texture* const texture) noexcept
	:
	m_bufferDesc(desc),
	m_resource(texture->GetUnderlyingResource())
{}
