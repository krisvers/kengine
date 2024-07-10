#ifndef KENGINE_CORE_ASSETS_TEXT_HPP
#define KENGINE_CORE_ASSETS_TEXT_HPP

#include <kengine/core/assets/asset.hpp>

namespace kengine::core::assets {

class TextAsset : public UUIDAsset<TextAsset> {
public:
	TextAsset() = default;
	~TextAsset() = default;

	bool load(std::string const& path) override {
		kengine::core::fileio::File<char> file;
		if (!file.load(path)) {
			return false;
		}

		_text = std::string(file.getData(), file.getBytesize());
		_isLoaded = true;
		file.unload();
		return true;
	}

	void unload() override {
		_text.clear();
		_isLoaded = false;
	}

	bool isLoaded() const override { return _isLoaded; }
	std::string const& getText() const { return _text; }

private:
	std::string _text;
	bool _isLoaded = false;
};

} // namespace kengine::core::assets

#endif