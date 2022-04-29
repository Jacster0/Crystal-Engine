#pragma once
#include "RHI/D3D12/D3D12DescriptorHeap.h"
#include "RHI/D3D12/D3D12PipelineState.h"
#include "Core/Math/Vector2.h"
#include <memory>
#include <d3d12.h>

#include "ComputeMipMaps.h"

namespace Crystal{
    struct alignas(16) GenerateMipsCB {
        uint32_t SrcMipLevel;
        uint32_t NumMipLevels;
        uint32_t SrcDimension;
        uint32_t IsSRGB;
        Math::Vector2 TexelSize;
    };

    enum class GenerateMips {
        GenerateMipsCB,
        SrcMip,
        OutMip,
        NumRootParameters
    }; 

    class RootSignature;
    class PipelineState;
    class ComputeMipsPass : public ComputePass {
    public:
        ComputeMipsPass(CommandContext& ctx);

        void Prepare() override;
        void Execute() override;

        [[nodiscard]] RootSignature& GetRootSignature() const noexcept { return *m_rootSignature; }
        [[nodiscard]] PipelineState& GetPipelineState() const noexcept { return *m_pipeLineState; }
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetDefaultUAV() const noexcept { return m_defaultUAV.GetDescriptorHandle(); }
    private:
        void CreateRootSignature() noexcept;
        void CreatePSO() noexcept;
        void CreateUAVS() noexcept;

        DescriptorAllocation m_defaultUAV;

        std::unique_ptr<RootSignature> m_rootSignature;
        std::unique_ptr<PipelineState> m_pipeLineState;
    };
}
