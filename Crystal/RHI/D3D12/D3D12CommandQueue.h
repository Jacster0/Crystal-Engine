#pragma once
#include "../Graphics/Types/Types.h"
#include <d3d12.h>
#include <wrl.h>

namespace Crystal {
	class CommandContext;
	class CommandQueue {
	public:
		explicit CommandQueue(const CommandListType& type);
		~CommandQueue();

		//Submits commands to be executed on the GPU
		uint64_t Submit(const CommandContext* const context);

		[[nodiscard]] uint64_t Signal() noexcept;
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3d12CommandQueue;
	};
}