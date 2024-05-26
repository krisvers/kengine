#ifndef KENGINE_CORE_ASSETS_MANAGER_HPP
#define KENGINE_CORE_ASSETS_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <memory>
#include <type_traits>

#include <kengine/types.hpp>
#include <kengine/singleton.hpp>
#include <kengine/core/exception.hpp>
#include <kengine/core/uuid.hpp>
#include <kengine/core/assets/asset.hpp>
#include <kengine/core/platform/memory.hpp>

namespace kengine::core::assets {

template<typename T>
class AssetReference : public UUID {
	~AssetReference() = default;

private:
	AssetReference() : UUID() {}

	friend class Manager;
};

class Manager : public kengine::Singleton<Manager> {
public:
	Manager() = default;
	~Manager() = default;

	template<typename T>
	AssetReference<T> load(std::string const& path) {
		static_assert(std::is_base_of<UUIDAsset<T>, T>::value, "T must be a subclass of UUIDAsset");

		auto it = _assets.find(path);
		if (it != _assets.end()) {
			if (it->second.asset->getUUID() != T::getUUIDStatic()) {
				throw Exception("Asset with path '{}' already loaded with different type", path);
			}

			++it->second.refCount;
			AssetReference<T>* r = static_cast<AssetReference<T>*>(&it->second.uuid);
			return *r;
		}

		AssetReference<T> ref = AssetReference<T>();
		T* asset = static_cast<T*>(kengine::core::platform::Memory::get().alloc(sizeof(T), platform::AllocationTag::Asset));
		new (asset) T();
		if (!asset->load(path)) {
			kengine::core::platform::Memory::get().dealloc(asset, sizeof(T));
			throw Exception("Failed to load asset with path '{}'", path);
		}

		_assets.emplace(path, ManagedAsset(ref, asset));
		return ref;
	}

	template<typename T>
	void unload(AssetReference<T> const& ref) {
		auto it = _assets.find(ref.toString());
		if (it == _assets.end()) {
			throw Exception("Asset with UUID '{}' not found", ref.toString());
		}

		if (it->second.asset->getUUID() != T::getUUIDStatic()) {
			throw Exception("Asset with UUID '{}' is not of type '{}'", ref.toString(), typeid(T).name());
		}

		if (it->second.refCount > 1) {
			--it->second.refCount;
			return;
		}

		it->second.asset->unload();
		kengine::core::platform::Memory::get().dealloc(it->second.asset, sizeof(T));
		_assets.erase(it);
	}

	template<typename T>
	T& getReference(AssetReference<T> const& ref) {
		auto it = _assets.find(ref.toString());
		if (it == _assets.end()) {
			throw Exception("Asset with UUID '{}' not found", ref.toString());
		}

		if (it->second.asset->getUUID() != T::getUUIDStatic()) {
			throw Exception("Asset with UUID '{}' is not of type '{}'", ref.toString(), typeid(T).name());
		}

		return dynamic_cast<T&>(*it->second.asset);
	}

	template<typename T>
	bool isLoaded(AssetReference<T> const& ref) {
		auto it = _assets.find(ref.toString());
		if (it == _assets.end()) {
			return false;
		}

		if (it->second.asset->getUUID() != T::getUUIDStatic()) {
			throw Exception("Asset with UUID '{}' is not of type '{}'", ref.toString(), typeid(T).name());
		}

		return it->second.asset->isLoaded();
	}

private:
	struct ManagedAsset {
		UUID& uuid;
		IAsset* asset = nullptr;
		kengine::usize refCount = 1;

		ManagedAsset(UUID& uuid, IAsset* asset) : uuid(uuid), asset(asset) {}
	};

	std::unordered_map<std::string, ManagedAsset> _assets;
};

} // namespace kengine::core::assets

#endif