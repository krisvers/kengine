#ifndef KENGINE_CORE_UUID_HPP
#define KENGINE_CORE_UUID_HPP

#include <random>

#include <kengine/types.hpp>
#include <kengine/singleton.hpp>

#ifdef uuid_t
#undef uuid_t
#endif

namespace kengine::core {

class UUIDs : public Singleton<UUIDs> {
public:
	UUIDs() = default;

	kengine::u64 generate() {
		static std::uniform_int_distribution<kengine::u64> dist(static_cast<kengine::u64>(2) << 61, static_cast<kengine::u64>(2) << 62);
		static std::mt19937_64 mt(rd());
		return dist(mt);
	}

private:
	std::random_device rd;
};

using uuid_t = kengine::u64;

class UUID {
public:
	UUID() { _uuid = UUIDs::get().generate(); }
	UUID(kengine::u64 uuid) : _uuid(uuid) {}
	UUID(UUID const& other) : _uuid(other._uuid) {}
	UUID(UUID&& other) noexcept : _uuid(other._uuid) {}

	UUID& operator=(UUID const& other) {
		_uuid = other._uuid;
		return *this;
	}

	UUID& operator=(UUID&& other) noexcept {
		_uuid = other._uuid;
		return *this;
	}

	bool operator==(UUID const& other) const { return _uuid == other._uuid; }
	bool operator!=(UUID const& other) const { return _uuid != other._uuid; }

	kengine::u64 getUUID() const { return _uuid; }
	std::string toString() const {
		std::stringstream ss;
		ss << std::hex << (_uuid >> 48) << "-" << ((_uuid >> 32) & 0xFFFF) << "-" << (_uuid & 0xFFFFFFFF);
		return ss.str();
	}

private:
	kengine::u64 _uuid = 0;
};

} // namespace kengine::core

#endif