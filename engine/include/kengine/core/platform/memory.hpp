#ifndef KENGINE_CORE_PLATFORM_MEMORY_HPP
#define KENGINE_CORE_PLATFORM_MEMORY_HPP

#include <kengine/types.hpp>
#include <kengine/core/logging.hpp>
#include <kengine/singleton.hpp>

#include <unordered_map>
#include <functional>

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

struct IMemoryType {
	kengine::u64 size;

	virtual const UUID& getUUID() const { return getUUIDStatic(); }
	virtual void dealloc(void* ptr) = 0;

	static const UUID& getUUIDStatic() {
		static UUID uuid = UUID(0);
		return uuid;
	}
};

template<typename T>
struct UUIDMemoryType : public IMemoryType {
	bool array = false;

	void dealloc(void* ptr) override {
		if (array) {
			Memory::get().deallocArray<T>(static_cast<T*>(ptr));
		} else {
			Memory::get().dealloc<T>(static_cast<T*>(ptr));
		}
	}

	UUIDMemoryType() {
		size = sizeof(T);
	}

	UUIDMemoryType(bool arr) {
		size = sizeof(T);
		array = arr;
	}

	const UUID& getUUID() const override { return getUUIDStatic(); }

	static const UUID& getUUIDStatic() {
		static UUID uuid = UUID();
		return uuid;
	}
};

class Memory : public Singleton<Memory> {
public:
	~Memory();

	template<typename T>
	T* alloc(AllocationTag tag) {
		T* ptr = new T();
		markAllocation(ptr, sizeof(T), tag, false, true, false, sizeof(T));

		UUIDMemoryType<T> type;
		for (kengine::usize i = 0; i < _memoryTypes.size(); ++i) {
			if (_memoryTypes[i]->getUUID() == type.getUUID()) {
				_allocations[ptr].typeIndex = i;
				return ptr;
			}
		}

		UUIDMemoryType<T>* pType = new UUIDMemoryType<T>();
		_memoryTypes.push_back(pType);

		_allocations[ptr].typeIndex = _memoryTypes.size() - 1;
		return ptr;
	}

	template<typename T>
	void dealloc(T* ptr) {
		if (ptr == nullptr) {
			throw kengine::core::Exception("Memory::dealloc<T>: Trying to deallocate null pointer");
		}

		if (_allocations.find(ptr) == _allocations.end()) {
			throw kengine::core::Exception("Memory::dealloc<T>: Trying to deallocate untracked pointer, could be a double-free or a garbage pointer");
		}

		if (!_allocations[ptr].typed) {
			throw kengine::core::Exception("Memory::dealloc<T>: Trying to deallocate pointer not template typed, therefore not allocated by Memory::alloc<T>, try using Memory::dealloc or Memory::deallocAligned instead");
		}

		if (_allocations[ptr].array) {
			throw kengine::core::Exception("Memory::dealloc<T>: Trying to deallocate pointer allocated by Memory::allocArray<T>, try using Memory::deallocArray<T>");
		}

		if (_allocations[ptr].aligned) {
			throw kengine::core::Exception("Memory::dealloc<T>: Trying to deallocate pointer allocated by Memory::allocAligned, try using Memory::deallocAligned instead");
		}
		
		UUIDMemoryType<T> type;
		if (_memoryTypes[_allocations[ptr].typeIndex]->getUUID() != type.getUUID()) {
			throw kengine::core::Exception("Memory::dealloc<T>: Trying to deallocate pointer with type index not matching templated type (allocated UUID {}, passed UUID {}), could be freeing the wrong allocation", _memoryTypes[_allocations[ptr].typeIndex]->getUUID().toString(), type.getUUID().toString());
		}

		delete ptr;
		unmarkAllocation(ptr);
	}

	template<typename T, kengine::usize N>
	T* allocArray(AllocationTag tag) {
		T* ptr = new T[N];
		markAllocation(ptr, sizeof(T) * N, tag, false, true, true, sizeof(T));

		UUIDMemoryType<T> type;
		for (kengine::usize i = 0; i < _memoryTypes.size(); ++i) {
			if (_memoryTypes[i]->getUUID() == type.getUUID()) {
				_allocations[ptr].typeIndex = i;
				return ptr;
			}
		}

		UUIDMemoryType<T>* pType = new UUIDMemoryType<T>(true);
		_memoryTypes.push_back(pType);

		_allocations[ptr].typeIndex = _memoryTypes.size() - 1;
		return ptr;
	}

	template<typename T>
	void deallocArray(T* ptr) {
		if (ptr == nullptr) {
			throw kengine::core::Exception("Memory::deallocArray<T>: Trying to deallocate null pointer");
		}

		if (_allocations.find(ptr) == _allocations.end()) {
			throw kengine::core::Exception("Memory::deallocArray<T>: Trying to deallocate untracked pointer, could be a double-free or a garbage pointer");
		}

		if (!_allocations[ptr].typed) {
			throw kengine::core::Exception("Memory::deallocArray<T>: Trying to deallocate pointer not template typed, therefore not allocated by Memory::allocArray<T>, try using Memory::alloc or Memory::allocAligned instead");
		}

		if (!_allocations[ptr].array) {
			throw kengine::core::Exception("Memory::deallocArray<T>: Trying to deallocate pointer allocated by Memory::alloc<T>, try using Memory::dealloc<T>");
		}

		if (_allocations[ptr].aligned) {
			throw kengine::core::Exception("Memory::deallocArray<T>: Trying to deallocate pointer allocated by Memory::allocAligned, try using Memory::deallocAligned instead");
		}
		
		UUIDMemoryType<T> type;
		if (_memoryTypes[_allocations[ptr].typeIndex]->getUUID() != type.getUUID()) {
			throw kengine::core::Exception("Memory::deallocArray<T>: Trying to deallocate pointer with type index not matching templated type (allocated UUID {}, passed UUID {}), could be freeing the wrong allocation", _memoryTypes[_allocations[ptr].typeIndex]->getUUID().toString(), type.getUUID().toString());
		}

		delete[] ptr;
		unmarkAllocation(ptr);
	}

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
	void markAllocation(void* ptr, kengine::u64 size, AllocationTag tag, bool aligned, bool typed, bool array, kengine::u64 alignedSize);
	void unmarkAllocation(void* ptr);

	struct AllocationEntry {
		kengine::u64 size;
		kengine::u64 alignedSize;
		AllocationTag tag;
		bool aligned;
		bool typed;
		bool array;

		kengine::usize typeIndex;
	};

	kengine::u64 _allocationSize;
	kengine::u64 _allocationCount;
	std::unordered_map<void*, AllocationEntry> _allocations;
	std::vector<IMemoryType*> _memoryTypes;
};

} // namespace kengine::core::platform

#endif