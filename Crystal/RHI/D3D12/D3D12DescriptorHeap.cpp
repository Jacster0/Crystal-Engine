#include "D3D12Core.h"
#include "D3D12DescriptorHeap.h"
#include "Utils/D3D12Exception.h"
#include <cassert>
#include <algorithm>

//Stupid fucking macros.
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

using namespace Crystal;

#pragma region DescriptorAllocatorPage

DescriptorAllocatorPage::DescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
	:
	m_heapType(type),
	m_numDescriptorsInHeap(numDescriptors)
{
	auto& device = RHICore::GetDevice();

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc {
		.Type           = m_heapType,
		.NumDescriptors = m_numDescriptorsInHeap
	};

	ThrowIfFailed(device.CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(&m_d3d12DescriptorHeap)));

	m_baseDescriptor                = m_d3d12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_descriptorHandleIncrementSize = device.GetDescriptorHandleIncrementSize(m_heapType);
	m_numFreeHandles                = m_numDescriptorsInHeap;

	// Initialize the free lists
	AddNewBlock(0, m_numFreeHandles);
}

DescriptorAllocation DescriptorAllocatorPage::Allocate(uint32_t numDescriptors) noexcept {
	std::scoped_lock lock(m_allocationMutex);

	// If There are less than the requested number of descriptors left in the heap,
	// return a NULL descriptor and try another heap
	if (numDescriptors > m_numFreeHandles) {
		return DescriptorAllocation();
	}

	//Get the first block that is large enough to satisfy the request
	const auto smallestBlockIterator = m_freeListBySize.lower_bound(numDescriptors);

	if (smallestBlockIterator == m_freeListBySize.end()) {
		// There was no free block that could satisfy the request
		return DescriptorAllocation();
	}

	//The size of the smallest block that satisfies the request
	const auto blockSize        = smallestBlockIterator->first;
	//The pointer to the same entry in the FreeListByOffset map
	const auto offsetIterator   = smallestBlockIterator->second;
	//The offset in the descriptor heap
	const auto offset           = offsetIterator->first;

	//Remove the existing free block from the free list
	m_freeListBySize.erase(smallestBlockIterator);
	m_freeListByOffset.erase(offsetIterator);

	//Compute the new free block that results from splitting this block
	const auto newOffset = offset + numDescriptors;
	const auto newSize   = blockSize - numDescriptors;

	if (newSize > 0) {
		// If the allocation didn't exactly match the requested size,
		// return the left-over to the free list
		AddNewBlock(newOffset, newSize);
	}

	m_numFreeHandles -= numDescriptors;

	return DescriptorAllocation(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(m_baseDescriptor, offset, m_descriptorHandleIncrementSize),
		numDescriptors,
		m_descriptorHandleIncrementSize,
		shared_from_this());
}

bool DescriptorAllocatorPage::HasSpace(uint32_t numDescriptors) const noexcept {
	return m_freeListBySize.lower_bound(numDescriptors) != m_freeListBySize.end();
}

void Crystal::DescriptorAllocatorPage::Free(DescriptorAllocation&& descriptor) noexcept {
	//Compute the offset of the descriptor within the descriptor heap
	const auto offset = ComputeOffset(descriptor.GetDescriptorHandle());

	std::scoped_lock lock(m_allocationMutex);
	//Don't add the block directly to the free list until the frame has completed
	m_staleDescriptors.emplace(offset, descriptor.GetNumHandles());
}

void Crystal::DescriptorAllocatorPage::ReleaseStaleDescriptors() noexcept {
	std::scoped_lock lock(m_allocationMutex);

	while (!m_staleDescriptors.empty()) {
		const auto& staleDescriptor = m_staleDescriptors.front();

		const auto offset           = staleDescriptor.Offset;
		const auto numDescriptors   = staleDescriptor.Size;

		FreeBlock(offset, numDescriptors);

		m_staleDescriptors.pop();
	}
}

uint32_t Crystal::DescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle) noexcept {
	return static_cast<uint32_t>(handle.ptr - m_baseDescriptor.ptr) / m_descriptorHandleIncrementSize;
}

void Crystal::DescriptorAllocatorPage::AddNewBlock(uint32_t offset, uint32_t numDescriptors) noexcept {
	const auto offsetIterator = m_freeListByOffset.emplace(offset, numDescriptors);
	const auto sizeIterator   = m_freeListBySize.emplace(numDescriptors, offsetIterator.first);

	offsetIterator.first->second.FreeListBySizeIt = sizeIterator;
}

void Crystal::DescriptorAllocatorPage::FreeBlock(uint32_t offset, uint32_t numDescriptors) noexcept {
	//Find the first element whose offset is greater than the specified offset.
	//This is the block that should appear after the block that is being freed.
	const auto nextBlockIterator = m_freeListByOffset.upper_bound(offset);

	//Find the block that appears before the block being freed
	auto prevBlockIterator = nextBlockIterator;

	//If it's not the first block in the list
	if (prevBlockIterator != m_freeListByOffset.begin()) {
		//Go to the previous block in the list
		--prevBlockIterator;
	}
	else {
		//Otherwise, set it to the end of the list to indicate that no
		//block comes before the one being freed
		prevBlockIterator = m_freeListByOffset.end();
	}

	//Add the number of free handles back to the heap.
	//This needs to be done before merging any blocks since merging
	//blocks modifies the numDescriptors variable.
	m_numFreeHandles += numDescriptors;

	if (prevBlockIterator != m_freeListByOffset.end() &&
		offset == prevBlockIterator->first + prevBlockIterator->second.Size) 
	{
		// The previous block is exactly behind the block that is to be freed.
		//
		//PrevBlock.Offset           Offset
		//|                          |
		//|<-----PrevBlock.Size----->|<------Size------>|

		//Increase the block size by the size of merging with the previous block
		offset = prevBlockIterator->first;
		numDescriptors += prevBlockIterator->second.Size;

		//Remove the previous block from the free list
		m_freeListBySize.erase(prevBlockIterator->second.FreeListBySizeIt);
		m_freeListByOffset.erase(prevBlockIterator);
	}

	if (nextBlockIterator != m_freeListByOffset.end() && offset + numDescriptors == nextBlockIterator->first) {
		// The next block is exactly in front of the block that is to be freed.
	  //
	  // Offset               NextBlock.Offset 
	  // |                    |
	  // |<------Size-------->|<-----NextBlock.Size----->|

	  //Increase the block size by the size of merging with the next block
		numDescriptors += nextBlockIterator->second.Size;

		//Remove the next block from the free list
		m_freeListBySize.erase(nextBlockIterator->second.FreeListBySizeIt);
		m_freeListByOffset.erase(nextBlockIterator);
	}

	//Add the freed block to the free list
	AddNewBlock(offset, numDescriptors);
}
#pragma endregion

#pragma region DescriptorAllocation

DescriptorAllocation::DescriptorAllocation()
	:
	m_descriptor{0},
	m_numHandles(0),
	m_descriptorSize(0),
	m_page(nullptr)
{}

DescriptorAllocation::DescriptorAllocation(
	D3D12_CPU_DESCRIPTOR_HANDLE descriptor, 
	uint32_t numHandles, 
	uint32_t descriptorSize, 
	std::shared_ptr<DescriptorAllocatorPage> page)
	:
	m_descriptor{descriptor},
	m_numHandles(numHandles),
	m_descriptorSize(descriptorSize),
	m_page(page)
{}

DescriptorAllocation::DescriptorAllocation(DescriptorAllocation&& allocation) noexcept 
	:
	m_descriptor(allocation.m_descriptor),
	m_numHandles(allocation.m_numHandles),
	m_descriptorSize(allocation.m_descriptorSize),
	m_page(allocation.m_page)
{
	allocation.m_descriptor.ptr = 0;
	allocation.m_numHandles     = 0;
	allocation.m_descriptorSize = 0;
}

DescriptorAllocation& DescriptorAllocation::operator=(DescriptorAllocation&& other) noexcept {
	// Free this descriptor if it points to anything.
	Free();

	m_descriptor = other.m_descriptor;
	m_numHandles = other.m_numHandles;
	m_descriptorSize = other.m_descriptorSize;
	m_page = std::move(other.m_page);

	other.m_descriptor.ptr = 0;
	other.m_numHandles = 0;
	other.m_descriptorSize = 0;

	return *this;
}

DescriptorAllocation::~DescriptorAllocation() { Free(); }

bool DescriptorAllocation::IsNull() const noexcept { return m_descriptor.ptr == 0; }

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetDescriptorHandle(uint32_t offset) const {
	assert(offset < m_numHandles);

	const auto index = m_descriptorSize * offset;

	return { m_descriptor.ptr + index };
}

void DescriptorAllocation::Free() noexcept {
	if (!IsNull() && m_page) {
		m_page->Free(std::move(*this));

		m_descriptor.ptr = 0;
		m_numHandles     = 0;
		m_descriptorSize = 0;
		m_page.reset();
	}
}
#pragma endregion

#pragma region DescriptorAllocator

Crystal::DescriptorAllocator::DescriptorAllocator(
	D3D12_DESCRIPTOR_HEAP_TYPE type, 
	uint32_t numDescriptors) noexcept
	:
	m_heapType(type),
	m_numDescriptorsPerHeap(numDescriptors)
{}

DescriptorAllocation Crystal::DescriptorAllocator::Allocate(uint32_t numDescriptors) noexcept {
	std::scoped_lock lock(m_allocationMutex);

	DescriptorAllocation allocation;

	auto iter = m_availableHeaps.begin();

	while (iter != m_availableHeaps.end()) {
		auto allocatorPage = m_heapPool[*iter];

		allocation = allocatorPage->Allocate(numDescriptors);

		if (allocatorPage->NumFreeHandles() == 0) {
			iter = m_availableHeaps.erase(iter);
		}
		else {
			iter++;
		}

		//A valid allocation has been found
		if (!allocation.IsNull()) {
			break;
		}
	}

	//No available heap could satisfy the requested number of descriptors
	if (allocation.IsNull()) {
		m_numDescriptorsPerHeap = std::max(m_numDescriptorsPerHeap, numDescriptors);
		auto newPage            = CreateAllocatorPage();

		allocation = newPage->Allocate(numDescriptors);
	}
	return allocation;
}

void Crystal::DescriptorAllocator::ReleaseStaleDescriptors() noexcept {
	std::scoped_lock lock(m_allocationMutex);

	for (size_t i = 0; i < m_heapPool.size(); i++) {
		auto page = m_heapPool[i];

		page->ReleaseStaleDescriptors();

		if (page->NumFreeHandles() > 0) {
			m_availableHeaps.insert(i);
		}
	}
}

std::shared_ptr<DescriptorAllocatorPage> Crystal::DescriptorAllocator::CreateAllocatorPage() noexcept {
	const auto newPage = std::make_shared<DescriptorAllocatorPage>(m_heapType, m_numDescriptorsPerHeap);

	m_heapPool.emplace_back(newPage);
	m_availableHeaps.insert(m_heapPool.size() - 1);

	return newPage;
}

#pragma endregion