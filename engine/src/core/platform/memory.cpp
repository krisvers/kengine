#include <kengine/core/platform/memory.hpp>
#include <kengine/core/exception.hpp>

#include <cstdlib>
#include <cstring>

namespace kengine::core::platform {

void* Memory::alloc(kengine::u64 size, AllocationTag tag) {
	if (size == 0) {
		throw kengine::core::Exception("Memory::alloc: Trying to allocate 0 bytes");
	}

	void* ptr = malloc(size);
	if (ptr == nullptr) {
		throw kengine::core::Exception("Memory::alloc Failed to allocate {} bytes", size);
	}

	_allocationSize += size;
	++_allocationCount;

	_allocations[ptr] = { size, tag, false };
	return ptr;
}

void Memory::dealloc(void* ptr, kengine::u64 size) {
	if (_allocations.find(ptr) == _allocations.end()) {
		throw kengine::core::Exception("Memory::dealloc: Trying to deallocate memory that was already freed or not allocated by Memory::alloc (possible double-free, or garbage pointer free)");
	}

	if (_allocations[ptr].size != size) {
		throw kengine::core::Exception("Memory::dealloc: Trying to deallocate memory with a different size than it was allocated with");
	}

	_allocationSize -= size;
	--_allocationCount;

	_allocations.erase(ptr);
	free(ptr);
}

void* Memory::allocAligned(kengine::u64 size, AllocationTag tag) {
	if (size == 0) {
		throw kengine::core::Exception("Memory::allocAligned: Trying to allocate 0 bytes");
	}

	kengine::u64 alignment = 16;
	kengine::u64 remainder = size % alignment;
	kengine::u64 newSize = size;
	if (remainder != 0) {
		newSize += alignment - remainder;
	}

	void* ptr = alloc(newSize, tag);

	AllocationEntry& entry = _allocations[ptr];
	entry.size = size;
	entry.aligned = true;
	return ptr;
}

void Memory::deallocAligned(void* ptr, kengine::u64 size) {
	if (size == 0) {
		throw kengine::core::Exception("Memory::deallocAligned: Trying to deallocate 0 bytes");
	}

	kengine::u64 alignment = 16;
	kengine::u64 remainder = size % alignment;
	if (remainder != 0) {
		size += alignment - remainder;
	}

	dealloc(ptr, size);
}

void Memory::copy(void* dest, const void* src, kengine::u64 size) {
	std::memcpy(dest, src, size);
}

void Memory::zero(void* dest, kengine::u64 size) {
	std::memset(dest, 0, size);
}

void Memory::set(void* dest, kengine::u8 value, kengine::u64 size) {
	std::memset(dest, value, size);
}

void Memory::move(void* dest, const void* src, kengine::u64 size) {
	std::memmove(dest, src, size);
}

void Memory::printAllocations(ILogger* logger, LogSeverity severity) {
	logger->logf(severity, "Memory allocations (total size {}, count {}):", _allocationSize, _allocationCount);

	for (kengine::s32 i = static_cast<kengine::s32>(AllocationTag::None); i < static_cast<kengine::s32>(AllocationTag::Max); ++i) {
		bool printed = false;
		kengine::u64 entryCount = 0;
		for (auto const& [ptr, entry] : _allocations) {
			if (entry.tag == static_cast<AllocationTag>(i)) {
				if (!printed) {
					logger->logf(severity, "  [{}]", allocationTagAsString(static_cast<AllocationTag>(i)));
					printed = true;
				}

				logger->logf(severity, "    {}:", entryCount);
				logger->logf(severity, "      address: 0x{}", ptr);
				logger->logf(severity, "      size: {} bytes", entry.size);
				logger->logf(severity, "      aligned: {}", entry.aligned);
				++entryCount;
			}
		}
	}
}

std::string Memory::allocationTagAsString(AllocationTag tag) {
	switch (tag) {
	case AllocationTag::None:
		return "None";
	case AllocationTag::String:
		return "String";
	case AllocationTag::File:
		return "File";
	case AllocationTag::Audio:
		return "Audio";
	case AllocationTag::Graphics:
		return "Graphics";
	case AllocationTag::Font:
		return "Font";
	case AllocationTag::Scene:
		return "Scene";
	case AllocationTag::Physics:
		return "Physics";
	case AllocationTag::Script:
		return "Script";
	case AllocationTag::Game:
		return "Game";
	case AllocationTag::Node:
		return "Node";
	case AllocationTag::DataStructure:
		return "DataStructure";
	case AllocationTag::Engine:
		return "Engine";
	case AllocationTag::Asset:
		return "Asset";
	case AllocationTag::_Internal:
		return "_Internal";
	case AllocationTag::Max:
	default:
		return "Unknown";
	}
}

} // namespace kengine::core