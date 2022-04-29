#pragma once

namespace  Crystal{
	class Texture;
	class CommandContext;
	class ComputePass {
	public:
        ComputePass(CommandContext& ctx) : m_ctx(ctx) { }

        ComputePass(const ComputePass& rhs) = delete;
		ComputePass& operator=(const ComputePass& rhs) = delete;
        virtual ~ComputePass() = default;

        virtual void Execute() = 0;
		virtual void Prepare() = 0;

		void SelectTexture(Texture& texture) { m_texture = &texture; }
	protected:
		Texture* m_texture{nullptr};
		CommandContext& m_ctx;
	};

}

