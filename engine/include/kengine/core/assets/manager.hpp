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
public:
	~AssetReference() {
		if (isLoaded()) {
			Manager::get().unload<T>(*this);
		}
	}

	AssetReference(AssetReference<T> const& other) : UUID(other) {
		if (!Manager::get().copyReference<T>(*this)) {
			throw Exception("Failed to copy AssetReference with UUID '{}'", other.toString());
		}
	}

	AssetReference& operator=(AssetReference const& other) {
		if (this == &other) {
			return *this;
		}

		Manager::get().unload<T>(*this);
		if (!Manager::get().copyReference<T>(other)) {
			throw Exception("Failed to copy AssetReference with UUID '{}'", other.toString());
		}

		UUID::operator=(other);
		return *this;
	}

	T& get() {
		return Manager::get().getReference<T>(*this);
	}

	bool isLoaded() {
		return Manager::get().isLoaded<T>(*this);
	}

	operator T&() {
		return get();
	}

	operator T&() const {
		return get();
	}

	AssetReference(AssetReference&&) = delete;
	AssetReference& operator=(AssetReference&&) = delete;

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

		T* asset = new T();
		if (!asset->load(path)) {
			delete asset;
			throw Exception("Failed to load asset with path '{}'", path);
		}

		AssetReference<T>& ref = *new AssetReference<T>();
		_assets.emplace(path, ManagedAsset(ref, asset));
		return ref;
	}

	template<typename T>
	void unload(AssetReference<T> const& ref) {
		for (auto& it : _assets) {
			if (it.second.uuid == ref) {
				if (it.second.asset->getUUID() != T::getUUIDStatic()) {
					throw Exception("Asset with UUID '{}' is not of type '{}' with uuid '{}'", ref.toString(), typeid(T).name(), T::getUUIDStatic());
				}

				if (it.second.refCount > 1) {
					--it.second.refCount;
					return;
				}

				it.second.asset->unload();
				delete it.second.asset;
				delete &ref;
				_assets.erase(it.first);
				return;
			}
		}

		throw Exception("Asset with UUID '{}' not found", ref.toString());
	}

	template<typename T>
	T& getReference(AssetReference<T> const& ref) {
		for (auto& it : _assets) {
			if (it.second.uuid == ref) {
				if (it.second.asset->getUUID() != T::getUUIDStatic()) {
					throw Exception("Asset with UUID '{}' is not of type '{}' with uuid '{}'", ref.toString(), typeid(T).name(), T::getUUIDStatic());
				}

				return dynamic_cast<T&>(*it.second.asset);
			}
		}

		throw Exception("Asset with UUID '{}' not found", ref.toString());
	}

	template<typename T>
	bool isLoaded(AssetReference<T> const& ref) {
		for (auto& it : _assets) {
			if (it.second.uuid == ref) {
				if (it.second.asset->getUUID() != T::getUUIDStatic()) {
					throw Exception("Asset with UUID '{}' is not of type '{}' with uuid '{}'", ref.toString(), typeid(T).name(), T::getUUIDStatic());
				}

				return it.second.asset->isLoaded();
			}
		}

		return false;
	}

	void unloadAll() {
		for (auto& it : _assets) {
			it.second.asset->unload();
			delete it.second.asset;
			delete &it.second.uuid;
		}

		_assets.clear();
	}

private:
	template<typename T>
	friend class AssetReference;

	template<typename T>
	bool copyReference(AssetReference<T> const& ref) {
		for (auto& it : _assets) {
			if (it.second.uuid == ref) {
				if (it.second.asset->getUUID() != T::getUUIDStatic()) {
					throw Exception("Asset with UUID '{}' is not of type '{}' with uuid '{}'", ref.toString(), typeid(T).name(), T::getUUIDStatic());
				}

				++it.second.refCount;
				return true;
			}
		}

		return false;
	}

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
