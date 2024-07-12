#ifndef KENGINE_CORE_ASSETS_IMAGE_HPP
#define KENGINE_CORE_ASSETS_IMAGE_HPP

#include <kengine/core/assets/asset.hpp>
#include <kengine/core/fileio/file.hpp>

namespace kengine::core::assets {

class ImageAsset : public UUIDAsset<ImageAsset> {
public:
	ImageAsset() = default;
	~ImageAsset() = default;

	bool load(std::string const& path) override {
		fileio::File<kengine::u8> file;
		if (!file.load(path)) {
			return false;
		}

		_isLoaded = true;
		file.unload();
		return true;
	}

	void unload() override {
		_isLoaded = false;
	}

	bool isLoaded() const override { return _isLoaded; }

private:
	bool _isLoaded = false;
};

} // namespace kengine::core::assets

#endif