#ifndef KENGINE_CORE_PLATFORM_MEMORY_HPP
#define KENGINE_CORE_PLATFORM_MEMORY_HPP

#include <kengine/types.hpp>
#include <kengine/core/logging.hpp>
#include <kengine/singleton.hpp>
#include <unordered_map>

namespace kengine::core::platform {

enum class AllocationTag {
	None = 0,
	String = 1,
	File = 2,
	Audio = 3,
	Graphics = 4,
	Font = 5,
	Scene = 6,
	Physics = 7,
	Script = 8,
	Game = 9,
	Node = 10,
	DataStructure = 11,
	Engine = 12,
	Asset = 13,
	Max = 14,

	_Internal = -1,
};

class Memory : public Singleton<Memory> {
public:
	~Memory();

	void* alloc(kengine::u64 size, AllocationTag tag);
	void dealloc(void* ptr, kengine::u64 size);

	void* allocAligned(kengine::u64 size, AllocationTag tag);
	void deallocAligned(void* ptr, kengine::u64 size);

	void copy(void* dest, const void* src, kengine::u64 size);
	void zero(void* dest, kengine::u64 size);
	void set(void* dest, kengine::u8 value, kengine::u64 size);
	void move(void* dest, const void* src, kengine::u64 size);

	kengine::u64 getAllocationSize() { return _allocationSize; }
	kengine::u64 getAllocationCount() { return _allocationCount; }
	void printAllocations(ILogger* logger, LogSeverity severity);
	std::string allocationTagAsString(AllocationTag tag);

private:
	struct AllocationEntry {
		kengine::u64 size;
		AllocationTag tag;
		bool aligned;
	};

	kengine::u64 _allocationSize;
	kengine::u64 _allocationCount;
	std::unordered_map<void*, AllocationEntry> _allocations;
};

} // namespace kengine::core::platform

#endif