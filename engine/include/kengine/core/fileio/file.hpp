#ifndef KENGINE_CORE_FILEIO_FILE_HPP
#define KENGINE_CORE_FILEIO_FILE_HPP

#include <string>
#include <fstream>

#include <kengine/types.hpp>
#include <kengine/core/platform/memory.hpp>

namespace kengine::core::fileio {

template<typename T>
class File {
public:
	File() = default;
	~File() = default;

	bool load(std::string const& path) {
		std::ifstream file(path);
		if (!file.is_open()) {
			return false;
		}

		file.seekg(0, std::ios::end);
		kengine::usize size = file.tellg();
		file.seekg(0, std::ios::beg);

		_bytesize = size;
		_data = reinterpret_cast<T*>(kengine::core::platform::Memory::get().alloc(sizeof(T) * size, platform::AllocationTag::File));

		file.read(reinterpret_cast<char*>(_data), size);
		file.close();

		_isLoaded = true;
		return true;
	}

	void unload() {
		if (!_isLoaded) {
			return;
		}

		kengine::core::platform::Memory::get().dealloc(_data, _bytesize);
		_data = nullptr;
		_bytesize = 0;
		_isLoaded = false;
	}

	bool isLoaded() const { return _isLoaded; }
	T const* getData() const { return _data; }
	kengine::usize getBytesize() const { return _bytesize; }

private:
	T* _data = nullptr;
	kengine::usize _bytesize = 0;
	bool _isLoaded = false;
};

} // namespace kengine::core::fileio

#endif