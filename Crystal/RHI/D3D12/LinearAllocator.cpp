#include "LinearAllocator.h"
#include "D3D12Core.h"

#include "Utils/d3dx12.h"
#include "Utils/D3D12Exception.h"

#include "../Core/Math/MathFunctions.h"

using namespace Crystal;

LinearAllocatorPage::LinearAllocatorPage(size_t sizeInBytes)
	:
	m_pageSize(sizeInBytes)
{
	auto& device = RHICore::get_device();

	const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const auto buffer    = CD3DX12_RESOURCE_DESC::Buffer(m_pageSize);

	ThrowIfFailed(device.CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&buffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_resource)));

	m_resource->SetName(L"Upload Buffer (Page)");

	m_gpuPtr = m_resource->GetGPUVirtualAddress();
	m_resource->Map(0, nullptr, &m_cpuPtr);
}

LinearAllocatorPage::~LinearAllocatorPage() {
	m_resource->Unmap(0, nullptr);
	m_cpuPtr = nullptr;
	m_gpuPtr = D3D12_GPU_VIRTUAL_ADDRESS(0);
}

bool LinearAllocatorPage::HasSpace(size_t sizeInBytes, size_t alignment) const noexcept {
	const size_t alignedSize   = Math::AlignUp(sizeInBytes, alignment);
	const size_t alignedOffset = Math::AlignUp(m_offset, alignment);

	return (alignedSize + alignedOffset) <= m_pageSize;
}

Allocation LinearAllocatorPage::Allocate(const size_t sizeInBytes, const size_t alignment) {
	if (!HasSpace(sizeInBytes, alignment)) {
		throw std::bad_alloc();
	}

	const size_t alignedSize = Math::AlignUp(sizeInBytes, alignment);
	m_offset                 = Math::AlignUp(m_offset, alignment);

	Allocation alloc{
		.CPU = static_cast<uint8_t*>(m_cpuPtr) + m_offset,
		.GPU = m_gpuPtr + m_offset
	};

	m_offset += alignedSize;

	return alloc;
}

void LinearAllocatorPage::Reset() noexcept {
	m_offset = 0;
}

LinearAllocator::LinearAllocator(size_t pageSize) noexcept
	:
	m_pageSize(pageSize)
{}

LinearAllocator::~LinearAllocator() {}

Allocation LinearAllocator::Allocate(const size_t sizeInBytes, const size_t alignment) {
	if (sizeInBytes < m_pageSize) {
		throw std::bad_alloc();
	}

	if (m_currentPage == nullptr || !m_currentPage->HasSpace(sizeInBytes, alignment)) {
		m_currentPage = RequestPage();
	}
	return m_currentPage->Allocate(sizeInBytes, alignment);
}

void LinearAllocator::Reset() noexcept {
	m_currentPage    = nullptr;
	m_availablePages = m_pagePool;

	for (auto page : m_availablePages) {
		page->Reset();
	}
}

std::shared_ptr<LinearAllocatorPage> LinearAllocator::RequestPage() noexcept {
	std::shared_ptr<LinearAllocatorPage> page;

	if (!m_availablePages.empty()) {
		page = m_availablePages.front();
		m_availablePages.pop_front();
	}
	else {
		page = std::make_shared<LinearAllocatorPage>(m_pageSize);
		m_pagePool.push_back(page);
	}
	return page;
}
