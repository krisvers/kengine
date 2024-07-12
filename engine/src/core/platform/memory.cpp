#include <kengine/core/platform/memory.hpp>
#include <kengine/core/exception.hpp>
#include <kengine/macros.hpp>

#include <cstdlib>
#include <cstring>

namespace kengine::core::platform {

Memory::~Memory() {
	if (_allocationCount > 0 || !_allocations.empty()) {
		std::stringstream sstream;
		Logger::get().logf(LogSeverity::Error, "Memory::~Memory: Memory leaks detected ({} bytes, {} allocations)", _allocationSize, _allocationCount);
		printAllocations(Logger::get().getLogger(), LogSeverity::Error);

		KENGINE_DEBUG_BREAK();

		// free all unfreed allocation
		for (const auto& [ptr, entry] : _allocations) {
			if (entry.typed) {
				_memoryTypes[entry.typeIndex]->dealloc(reinterpret_cast<void*>(ptr));
			} else if (entry.aligned) {
				deallocAligned(ptr, entry.size);
			} else {
				dealloc(ptr, entry.size);
			}
		}
	}
}

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

	markAllocation(ptr, size, tag, false, false, false, size);
	return ptr;
}

void Memory::dealloc(void* ptr, kengine::u64 size) {
	if (_allocations.find(ptr) == _allocations.end()) {
		throw kengine::core::Exception("Memory::dealloc: Trying to deallocate memory that was already freed or not allocated by Memory::alloc (possible double-free, or garbage pointer free)");
	}

	if (_allocations[ptr].size != size) {
		throw kengine::core::Exception("Memory::dealloc: Trying to deallocate memory with a different size than it was allocated with");
	}

	if (_allocations[ptr].aligned) {
		throw kengine::core::Exception("Memory::dealloc: Trying to deallocate aligned memory with Memory::dealloc, use Memory::deallocAligned instead");
	}

	if (_allocations[ptr].typed) {
		throw kengine::core::Exception("Memory::dealloc: Trying to deallocate typed memory with Memory::dealloc, use Memory::dealloc<T> instead");
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

	void* ptr = malloc(newSize);
	if (ptr == nullptr) {
		throw kengine::core::Exception("Memory::allocAligned Failed to allocate {} bytes", newSize);
	}

	_allocationSize += newSize;
	++_allocationCount;

	markAllocation(ptr, size, tag, true, false, false, newSize);
	return ptr;
}

void Memory::deallocAligned(void* ptr, kengine::u64 size) {
	if (size == 0) {
		throw kengine::core::Exception("Memory::deallocAligned: Trying to deallocate 0 bytes");
	}

	if (_allocations.find(ptr) == _allocations.end()) {
		throw kengine::core::Exception("Memory::deallocAligned: Trying to deallocate memory that was already freed or not allocated by Memory::allocAligned (possible double-free, or garbage pointer free)");
	}

	if (_allocations[ptr].typed) {
		throw kengine::core::Exception("Memory::deallocAligned: Trying to deallocate typed memory with Memory::deallocAligned, use Memory::dealloc<T> instead");
	}

	if (!_allocations[ptr].aligned) {
		throw kengine::core::Exception("Memory::deallocAligned: Trying to deallocate non-aligned memory with Memory::deallocAligned, use Memory::dealloc instead");
	}

	if (_allocations[ptr].size != size) {
		throw kengine::core::Exception("Memory::deallocAligned: Trying to deallocate memory with a different size than it was allocated with");
	}

	kengine::u64 alignment = 16;
	kengine::u64 remainder = size % alignment;
	kengine::u64 alignedSize = size;
	if (remainder != 0) {
		alignedSize += alignment - remainder;
	}

	if (_allocations[ptr].alignedSize != alignedSize) {
		throw kengine::core::Exception("Memory::deallocAligned: Trying to deallocate memory with a different aligned size than it was allocated with");
	}
	
	_allocationSize -= alignedSize;
	--_allocationCount;

	_allocations.erase(ptr);
	free(ptr);
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
		for (const auto& [ptr, entry] : _allocations) {
			if (entry.tag == static_cast<AllocationTag>(i)) {
				if (!printed) {
					logger->logf(severity, "  [{}]", allocationTagAsString(static_cast<AllocationTag>(i)));
					printed = true;
				}

				logger->logf(severity, "    {}:", entryCount);
				logger->logf(severity, "      address: {}", ptr);
				logger->logf(severity, "      size: {} bytes", entry.size);
				logger->logf(severity, "      aligned: {}", entry.aligned);
				if (entry.aligned) {
					logger->logf(severity, "      aligned size: {} bytes", entry.alignedSize);
				}
				logger->logf(severity, "      typed: {}", entry.typed);
				logger->logf(severity, "      array: {}", entry.array);
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

void Memory::markAllocation(void* ptr, kengine::u64 size, AllocationTag tag, bool aligned, bool typed, bool array, kengine::u64 alignedSize) {
	_allocations[ptr] = { size, alignedSize, tag, aligned, typed, array };
}

void Memory::unmarkAllocation(void* ptr) {
	_allocations.erase(ptr);
}

} // namespace kengine::core
