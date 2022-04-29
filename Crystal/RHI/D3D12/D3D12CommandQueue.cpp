#include "D3D12CommandQueue.h"
#include "D3D12Core.h"
#include "D3D12CommandContext.h"
#include "Utils/D3D12Exception.h"

#include <limits>
#include <array>
#include <algorithm>

#include "Core/Logging/Logger.h"

using namespace Crystal;

CommandQueue::CommandQueue(CommandListType_t cmdListType) {
	CommandListType type{ .Type = cmdListType };

	auto& device = RHICore::get_device();

	const D3D12_COMMAND_QUEUE_DESC desc {
		.Type     = type.As<D3D12_COMMAND_LIST_TYPE>(),
		.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
		.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
		.NodeMask = 0
	};

	ThrowIfFailed(device.CreateCommandQueue(&desc, IID_PPV_ARGS(&m_d3d12CommandQueue)));
	ThrowIfFailed(device.CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

	if (type.Type == CommandListType_t::copy) {
		m_d3d12CommandQueue->SetName(L"Copy Command Queue");
		m_context = std::make_unique<CommandContext>(type);
	}
	else if (type.Type == CommandListType_t::compute) {
		m_d3d12CommandQueue->SetName(L"Compute Command Queue");
		m_context = std::make_unique<ComputeContext>(type);
	}
	else if (type.Type == CommandListType_t::direct) {
		m_d3d12CommandQueue->SetName(L"Direct Command Queue");
		m_context = std::make_unique<GraphicsContext>(type);
	}
}

void CommandQueue::Submit(CommandContext* const context) {
    Submit(std::array { context });
}

void CommandQueue::Submit(std::span<CommandContext* const> contexts) {
	namespace rn = std::ranges;

	std::vector<ID3D12CommandList*> commandLists;

	//commandLists.emplace_back((* contexts.begin())->GetNativeCommandList().Get());

	for (const auto ctx : contexts) {
		ctx->Close();
		commandLists.emplace_back(ctx->GetNativeCommandList().Get());
	}

	m_d3d12CommandQueue->ExecuteCommandLists(1, commandLists.data());

	WaitForFenceValue(Signal());

	for(const auto ctx : contexts) {
		ctx->Reset();
	}
}

uint64_t CommandQueue::Signal() {
	const auto fenceValue = ++m_fenceValue;
	ThrowIfFailed(m_d3d12CommandQueue->Signal(m_fence.Get(), fenceValue));

	return fenceValue;
}

void CommandQueue::WaitForFenceValue(uint64_t fenceValue) const {
	if (!IsFenceComplete(fenceValue)) {
		if (const auto event = CreateEvent(nullptr, false, false, nullptr)) {
			ThrowIfFailed(m_fence->SetEventOnCompletion(fenceValue, event));

			WaitForSingleObject(event, std::numeric_limits<DWORD>::max());
			CloseHandle(event);
		}
	}
}

bool CommandQueue::IsFenceComplete(uint64_t fenceValue) const noexcept {
	return m_fence->GetCompletedValue() >= fenceValue;
}

void CommandQueue::Wait(const CommandQueue& rhs) const {
	ThrowIfFailed(m_d3d12CommandQueue->Wait(rhs.m_fence.Get(), rhs.m_fenceValue));
}
