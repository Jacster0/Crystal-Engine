#pragma once
#include "Utils/d3dx12.h"

namespace Crystal {
	struct PipelineStateSpecification {
		size_t sizeInBytes;
		void* pipeLineStateStream;
	};
	class PipelineState {
	public:
		explicit PipelineState(const PipelineStateSpecification& specification) noexcept;
		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12PipelineState> GetNativePipelineState() const noexcept;
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelinestate;
	};
}