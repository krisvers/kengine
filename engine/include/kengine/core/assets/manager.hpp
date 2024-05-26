#ifndef KENGINE_CORE_ASSETS_MANAGER_HPP
#define KENGINE_CORE_ASSETS_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <memory>
#include <type_traits>

#include <kengine/types.hpp>
#include <kengine/singleton.hpp>
#include <kengine/core/assets/asset.hpp>

namespace kengine::core::assets {

class Manager : public kengine::Singleton<Manager> {
public:
	Manager() = default;
	~Manager() = default;

	template<typename T>
	std::shared_ptr<T> load(std::string const& path) {
		static_assert(std::is_base_of<UUIDAsset<T>, T>::value, "T must be a subclass of UUIDAsset");

		auto it = _assets.find(path);
		if (it != _assets.end()) {
			if (it->second->getUUID() != T::getUUIDStatic()) {
				throw Exception("Asset with path '{}' already loaded with different type", path);
			}

			return std::static_pointer_cast<T>(it->second);
		}

		auto asset = std::make_shared<T>();
		if (!asset->load(path)) {
			return nullptr;
		}

		_assets[path] = asset;
		return asset;
	}

	template<typename T>
	std::shared_ptr<T> getAsset(std::string const& path) {
		static_assert(std::is_base_of<UUIDAsset<T>, T>::value, "T must be a subclass of UUIDAsset");

		auto it = _assets.find(path);
		if (it != _assets.end()) {
			if (it->second->getUUID() != T::getUUIDStatic()) {
				throw Exception("Asset with path '{}' and UUID '{}' has wrong type; expected UUID '{}'", path, it->second->getUUID(), T::getUUIDStatic());
			}

			return std::static_pointer_cast<T>(it->second);
		}

		return nullptr;
	}

	std::shared_ptr<IAsset> getAsset(std::string const& path) {
		auto it = _assets.find(path);
		if (it != _assets.end()) {
			return it->second;
		}

		return nullptr;
	}

	template<typename T>
	void release(std::string const& path) {
		static_assert(std::is_base_of<UUIDAsset<T>, T>::value, "T must be a subclass of UUIDAsset");

		auto it = _assets.find(path);
		if (it == _assets.end()) {
			return;
		}

		if (it->second->getUUID() != T::getUUIDStatic()) {
			throw Exception("Asset with path '{}' and UUID '{}' has wrong type; expected UUID '{}'", path, it->second->getUUID(), T::getUUIDStatic());
		}

		if (it->second.use_count() > 1) {
			return;
		}

		it->second->unload();
		_assets.erase(it);
	}

	template<typename T>
	void release(std::shared_ptr<T> asset) {
		static_assert(std::is_base_of<UUIDAsset<T>, T>::value, "T must be a subclass of UUIDAsset");

		auto it = _assets.begin();
		while (it != _assets.end()) {
			if (it->second == asset) {
				if (it->second->getUUID() != T::getStaticUUID()) {
					throw Exception("Asset with path '{}' and UUID '{}' has wrong type; expected UUID '{}'", it->first, it->second->getUUID(), T::getStaticUUID());
				}

				it->second->unload();
				it = _assets.erase(it);
			} else {
				++it;
			}
		}
	}

	void release(std::string const& path) {
		auto it = _assets.find(path);
		if (it == _assets.end()) {
			return;
		}

		if (it->second.use_count() > 1) {
			return;
		}

		it->second->unload();
		_assets.erase(it);
	}

	void release(std::shared_ptr<IAsset> asset) {
		auto it = _assets.begin();
		while (it != _assets.end()) {
			if (it->second == asset) {
				it->second->unload();
				it = _assets.erase(it);
			} else {
				++it;
			}
		}
	}

	template<typename T>
	void unloadAll() {
		static_assert(std::is_base_of<UUIDAsset<T>, T>::value, "T must be a subclass of UUIDAsset");

		for (auto& asset : _assets) {
			if (asset.second->getUUID() == T::getUUIDStatic()) {
				asset.second->unload();
			}
		}
	}

	void unloadAll() {
		for (auto& asset : _assets) {
			asset.second->unload();
		}
	}

private:
	std::unordered_map<std::string, std::shared_ptr<IAsset>> _assets;
};

} // namespace kengine::core::assets

#endif