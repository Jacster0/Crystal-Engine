#include "D3D12RootSignature.h"
#include "D3D12Core.h"
#include "Utils/D3D12Exception.h"

using namespace Crystal;
using namespace Microsoft::WRL;

RootSignature::RootSignature() {}

RootSignature::~RootSignature() {
	Destroy();
}

void RootSignature::Intialize(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc) {
	SetRootSignatureDesc(rootSignatureDesc);
}

void RootSignature::Destroy() {
	for (uint32_t i = 0; i < m_rootSignatureDesc.NumParameters; i++) {
		const auto& rootParam = m_rootSignatureDesc.pParameters[i];

		if (rootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
			delete[] rootParam.DescriptorTable.pDescriptorRanges;
		}
	}

	delete[] m_rootSignatureDesc.pParameters;
	m_rootSignatureDesc.pParameters   = nullptr;
	m_rootSignatureDesc.NumParameters = 0;

	delete[] m_rootSignatureDesc.pStaticSamplers;
	m_rootSignatureDesc.pStaticSamplers   = nullptr;
	m_rootSignatureDesc.NumStaticSamplers = 0;

	m_descriptorTableBitMask = 0;
	m_samplerBitMask         = 0;

	ZeroMemory(m_numDescriptorsPerTable.data(), sizeof(m_numDescriptorsPerTable));
}

uint32_t Crystal::RootSignature::GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) const {
	uint32_t descriptorTableBitMask = 0;

	switch (descriptorHeapType) {
	case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
		descriptorTableBitMask = m_descriptorTableBitMask;
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
		descriptorTableBitMask = m_samplerBitMask;
		break;
	}

	return descriptorTableBitMask;
}

void RootSignature::SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc) {
	// Make sure any previously allocated root signature description is cleaned up first.
	Destroy();

	const auto numParams = rootSignatureDesc.NumParameters;
	D3D12_ROOT_PARAMETER1* params{ nullptr };

	if (numParams > 0) {
		params = new D3D12_ROOT_PARAMETER1[numParams];
	}

	for (uint32_t i = 0; i < numParams; i++) {
		const auto& rootParam = rootSignatureDesc.pParameters[i];
		params[i]             = rootParam;

		if (rootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
			const auto numDescriptorRanges = rootParam.DescriptorTable.NumDescriptorRanges;

			D3D12_DESCRIPTOR_RANGE1* descriptorRanges{ nullptr };

			if (numDescriptorRanges > 0) {
				descriptorRanges = new D3D12_DESCRIPTOR_RANGE1[numDescriptorRanges];
			}

			std::memcpy(descriptorRanges, rootParam.DescriptorTable.pDescriptorRanges, sizeof(D3D12_DESCRIPTOR_RANGE1) * numDescriptorRanges);

			params[i].DescriptorTable.NumDescriptorRanges = numDescriptorRanges;
			params[i].DescriptorTable.pDescriptorRanges   = descriptorRanges;

			if (numDescriptorRanges > 0) {
				switch (descriptorRanges[0].RangeType) {
				case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
				case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
				case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
					m_descriptorTableBitMask |= (1 << i);
					break;
				case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
					m_samplerBitMask |= (1 << i);
					break;
				}
			}

			for (uint32_t j = 0; j < numDescriptorRanges; j++) {
				m_numDescriptorsPerTable[i] += descriptorRanges[j].NumDescriptors;
			}
		}
	}

	auto numStaticSamplers = rootSignatureDesc.NumStaticSamplers;
	D3D12_STATIC_SAMPLER_DESC* staticSamplers =
		numStaticSamplers > 0
		? new D3D12_STATIC_SAMPLER_DESC[numStaticSamplers]
		: nullptr;

	if (staticSamplers) {
		memcpy(
			staticSamplers,
			rootSignatureDesc.pStaticSamplers,
			sizeof(D3D12_STATIC_SAMPLER_DESC) * numStaticSamplers);
	}

	m_rootSignatureDesc.NumParameters     = numParams;
	m_rootSignatureDesc.pParameters       = params;
	m_rootSignatureDesc.NumStaticSamplers = numStaticSamplers;
	m_rootSignatureDesc.pStaticSamplers   = staticSamplers;
	m_rootSignatureDesc.Flags             = rootSignatureDesc.Flags;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc;
	versionedRootSignatureDesc.Init_1_1(numParams, params, numStaticSamplers, staticSamplers, rootSignatureDesc.Flags);

	D3D_ROOT_SIGNATURE_VERSION highestVersion = RHICore::GetHighestRootSignatureVersion();

	// Serialize the root signature.
	ComPtr<ID3DBlob> rootSignatureBlob;
	ComPtr<ID3DBlob> errorBlob;
	
	auto& device = RHICore::GetDevice();

	
	ThrowIfFailed(device.CreateRootSignature(
		0,
		rootSignatureBlob->GetBufferPointer(),
		rootSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&m_rootSignature)));
}
