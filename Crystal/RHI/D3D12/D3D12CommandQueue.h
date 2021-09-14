#pragma once

#include "../Graphics/Types/Types.h"

#include <d3d12.h>
#include <wrl.h>
#include <atomic>
#include <memory>
#include <span>

namespace Crystal {
	class CommandContext;
	class CommandQueue {
	public:
		explicit CommandQueue(CommandListType_t cmdListType);

		//Submits commands to be executed on the GPU
		uint64_t Submit(const CommandContext const* context);
		uint64_t Submit(std::span<const CommandContext* const> contexts);

		[[nodiscard]] uint64_t Signal();
		void WaitForFenceValue(uint64_t fenceValue);

		[[nodiscard]] bool IsFenceComplete(uint64_t fenceValue) const noexcept;
		void Wait(const CommandQueue& rhs) const;

		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetNativeCommandQueue() const noexcept { return m_d3d12CommandQueue; }
		[[nodiscard]] const CommandContext& GetCommandList() const noexcept { return *m_context.get(); }
		[[nodiscard]] CommandContext& GetCommandContext() noexcept { return *m_context.get(); }
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3d12CommandQueue;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		std::unique_ptr<CommandContext> m_context;
		std::atomic_uint64_t  m_fenceValue;
	};
}