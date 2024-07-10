#ifndef KENGINE_CORE_ASSETS_ASSET_HPP
#define KENGINE_CORE_ASSETS_ASSET_HPP

#include <string>
#include <memory>

#include <kengine/types.hpp>
#include <kengine/core/uuid.hpp>
#include <kengine/core/fileio/file.hpp>

namespace kengine::core::assets {

class IAsset {
public:
	IAsset() = default;
	virtual ~IAsset() = default;

	virtual bool load(std::string const& path) = 0;
	virtual void unload() = 0;

	virtual bool isLoaded() const = 0;

	virtual const UUID& getUUID() const {
		static UUID uuid = UUID(0);
		return uuid;
	}
};

template<typename T>
class UUIDAsset : public IAsset {
public:
	const UUID& getUUID() const override { return getUUIDStatic(); }

	static const UUID& getUUIDStatic() {
		static UUID uuid = UUID();
		return uuid;
	}
};

} // namespace kengine::core::assets

#endif