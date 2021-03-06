#include "CpuInfo.h"
#include <algorithm>

using namespace Crystal;

CpuInfo::CpuInfo() noexcept {
    GetNativeSystemInfo(&m_sysInfo);

    Info.BrandString          = GetBrandString();
    Info.Vendor               = GetVendor();
    Info.Architecture         = GetArchitecture();
    Info.NumLogicalProcessors = GetNumberOfLogicalProcessors();
    Info.NumCores             = GetNumberOfCores();

    Info.InstructionSetFeatures = {
       {"SSE3",        m_instructionSet.SSE3()},
       {"PCLMULQDQ",   m_instructionSet.PCLMULQDQ()},
       {"MONITOR",     m_instructionSet.MONITOR()},
       {"FMA",         m_instructionSet.FMA()},
       {"CMPXCHG16B",  m_instructionSet.CMPXCHG16B()},
       {"SSE4.1",      m_instructionSet.SSE41()},
       {"SSE4.2",      m_instructionSet.SSE42()},
       {"MOVBE",       m_instructionSet.MOVBE()},
       {"POPCNT",      m_instructionSet.POPCNT()},
       {"AES",         m_instructionSet.AES()},
       {"XSAVE",       m_instructionSet.XSAVE()},
       {"OSXSAVE",     m_instructionSet.OSXSAVE()},
       {"AVX",         m_instructionSet.AVX()},
       {"F16C",        m_instructionSet.F16C()},
       {"RDRAND",      m_instructionSet.RDRAND()},
       {"MSR",         m_instructionSet.MSR()},
       {"CX8",         m_instructionSet.CX8()},
       {"SEP",         m_instructionSet.SEP()},
       {"CMOV",        m_instructionSet.CMOV()},
       {"CLFSH",       m_instructionSet.CLFSH()},
       {"MMX",         m_instructionSet.MMX()},
       {"FXSR",        m_instructionSet.FXSR()},
       {"SSE",         m_instructionSet.SSE()},
       {"SSE2",        m_instructionSet.SSE2()},
       {"FSGSBASE",    m_instructionSet.FSGSBASE()},
       {"BMI1",        m_instructionSet.BMI1()},
       {"HLE",         m_instructionSet.HLE()},
       {"AVX2",        m_instructionSet.AVX2()},
       {"BMI2",        m_instructionSet.BMI2()},
       {"ERMS",        m_instructionSet.ERMS()},
       {"INVPCID",     m_instructionSet.INVPCID()},
       {"RTM",         m_instructionSet.RTM()},
       {"AVX512F",     m_instructionSet.AVX512F()},
       {"RDSEED",      m_instructionSet.RDSEED()},
       {"ADX",         m_instructionSet.ADX()},
       {"AVX512PF",    m_instructionSet.AVX512PF()},
       {"AVX512ER",    m_instructionSet.AVX512ER()},
       {"AVX512CD",    m_instructionSet.AVX512CD()},
       {"SHA",         m_instructionSet.SHA()},
       {"PREFETCHWT1", m_instructionSet.PREFETCHWT1()},
       {"LAHF",        m_instructionSet.LAHF()},
       {"LZCNT",       m_instructionSet.LZCNT()},
       {"ABM",         m_instructionSet.ABM()},
       {"SSE4a",       m_instructionSet.SSE4a()},
       {"XOP",         m_instructionSet.XOP()},
       {"TBM",         m_instructionSet.TBM()},
       {"SYSCALL",     m_instructionSet.SYSCALL()},
       {"MMXEXT",      m_instructionSet.MMXEXT()},
       {"RDTSCP",      m_instructionSet.RDTSCP()},
       {"3DNOWEXT",    m_instructionSet._3DNOWEXT()},
       {"3DNOW",       m_instructionSet._3DNOW()}
    };
}

uint32_t CpuInfo::GetNumberOfCores() noexcept {
    DWORD sizeInBytes{ 0 };
    auto numCores{ 0 };

    //Retrieve the buffer length
    GetLogicalProcessorInformation(nullptr, &sizeInBytes);
    //Calculate number of elements in the buffer based on the returned size
    const size_t numElements = sizeInBytes / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);

    std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(numElements);

    //Fill the buffer. If the function succeeds we can start counting cores.
    if (GetLogicalProcessorInformation(buffer.data(), &sizeInBytes)) [[likely]] {
        std::ranges::for_each(buffer, [&numCores](const SYSTEM_LOGICAL_PROCESSOR_INFORMATION& processorInfo) {
            if (processorInfo.Relationship == RelationProcessorCore) {
                numCores++;
            }
        });
    }
    return numCores;
}
