#include "D3D12CommandQueue.h"
#include "D3D12CommandContext.h"
#include <array>


using namespace Crystal;

uint64_t CommandQueue::Submit(const CommandContext* const context) {
	std::array<ID3D12CommandList*, 1> commandList = { context->GetNativeCommandList().Get() };

	m_d3d12CommandQueue->ExecuteCommandLists(commandList.size(), commandList.data());

	uint64_t fenceValue = Signal();

	return fenceValue;
}

uint64_t CommandQueue::Signal() noexcept
{
	return uint64_t();
}
