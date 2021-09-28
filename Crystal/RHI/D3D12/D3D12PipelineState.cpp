#include "D3D12Core.h"
#include "D3D12PipelineState.h"

#include "Utils/D3D12Exception.h"

using namespace Crystal;
using namespace Microsoft::WRL;

PipelineState::PipelineState(const PipelineStateSpecification& specification) noexcept {
	auto& device = RHICore::get_device();

	ThrowIfFailed(device.CreatePipelineState(
		reinterpret_cast<const D3D12_PIPELINE_STATE_STREAM_DESC*>(&specification), 
		IID_PPV_ARGS(&m_pipelinestate)));
}

ComPtr<ID3D12PipelineState> PipelineState::GetNativePipelineState() const noexcept {
	return m_pipelinestate;
}
