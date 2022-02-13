#pragma once
#include "Platform/Windows/CrystalWindow.h"
#include "InstructionSet.h"
#include <unordered_map>
#include <thread>

namespace Crystal {
    struct CpuInfo_t {
        std::string BrandString{};
        std::string Vendor{};
        std::string Architecture{};
        int NumLogicalProcessors{};
        int NumCores{};
        std::unordered_map<std::string, bool> InstructionSetFeatures{};
    };

    class CpuInfo {
    public:
        CpuInfo() noexcept;

        CpuInfo_t Info;
    private:
        [[nodiscard]] std::string GetBrandString()  const noexcept { return m_instructionSet.Brandstring(); }
        [[nodiscard]] std::string GetVendor()       const noexcept { return m_instructionSet.Vendor(); }
        [[nodiscard]] std::string GetArchitecture() const noexcept { return m_architectures.at(m_sysInfo.wProcessorArchitecture); }

        [[nodiscard]] static uint32_t GetNumberOfLogicalProcessors() noexcept { return std::thread::hardware_concurrency(); }
        [[nodiscard]] static uint32_t GetNumberOfCores() noexcept;

        SYSTEM_INFO m_sysInfo{};
        InstructionSet m_instructionSet;
        std::unordered_map<int, std::string> m_architectures{
            {9, "x86-64"},
            {5, "ARM"},
            {12, "ARM64"},
            {6, "Intel Itanium-based"},
            {0, "x86-32"},
            {0xFFFF, "Unknown architecture."}
        };
    };
}