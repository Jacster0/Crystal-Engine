#pragma once
#include "../Graphics/Types/Types.h"
#include <d3d12.h>
#include <wrl.h>
#include <atomic>
#include <memory>
#include <type_traits>
#include <array>
#include <limits>

#include "D3D12CommandContext.h"
#include "Utils/ResourceStateTracker.h"
#include "../Core/Lib/type_traits.h"

namespace Crystal {
	template<CommandListType_t CommandType = CommandListType_t::direct>
	class CommandQueue {
	public:
		CommandQueue() {
			if constexpr (CommandType == CommandListType_t::copy) {
				m_context = std::make_unique<CommandContext>(Type);
			}
			else if constexpr (CommandType == CommandListType_t::compute) {
				m_context = std::make_unique<ComputeContext>(Type);
			}
			else if constexpr (CommandType == CommandListType_t::direct) {
				m_context = std::make_unique<GraphicsContext>(Type);
			}
		}

		//Submits commands to be executed on the GPU
		uint64_t Submit(const CommandContext* const context) {
			std::array<ID3D12CommandList*, 1> commandList = { context->GetNativeCommandList().Get() };

			m_d3d12CommandQueue->ExecuteCommandLists(commandList.size(), commandList.data());

			uint64_t fenceValue = Signal();

			return fenceValue;
		}

		[[nodiscard]] uint64_t Signal() noexcept {
			const auto fenceValue = ++m_fenceValue;
			m_d3d12CommandQueue->Signal(m_fence, fenceValue);

			return fenceValue;
		}
		void WaitForFenceValue(uint64_t fenceValue) noexcept {
			if (!IsFenceComplete(fenceValue)) {
				auto event = CreateEvent(nullptr, false, false, nullptr);

				m_fence->SetEventOnCompletion(fenceValue, event);

				WaitForSingleObject(event, std::numeric_limits<DWORD>::max());
				CloseHandle();
			}
		}

		[[nodiscard]] bool IsFenceComplete(uint64_t fenceValue) noexcept { return m_fence->GetCompletedValue() >= fenceValue; }
		void Wait(const CommandQueue& rhs) noexcept { m_d3d12CommandQueue->Wait(rhs.m_fence.Get(), rhs.m_fenceValue); }

		[[nodiscard]] ID3D12CommandQueue& GetNativeCommandQueue() noexcept { *return m_d3d12CommandQueue.Get(); }
		[[nodiscard]] const ID3D12CommandQueue& GetNativeCommandQueue() const noexcept { *return m_d3d12CommandQueue.Get(); }

		[[nodiscard]] const CommandContext& GetCommandList() const noexcept { return *m_context.get(); }
		[[nodiscard]] CommandContext& GetCommandList() noexcept { return *m_context.get(); }
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3d12CommandQueue;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		std::unique_ptr<CommandContext> m_context;
		std::atomic_uint64_t  m_fenceValue;
	};
}