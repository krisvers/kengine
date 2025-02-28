#ifndef KENGINE_CORE_GRAPHICS_SHADER_HPP
#define KENGINE_CORE_GRAPHICS_SHADER_HPP

#include <kengine/core/assets/asset.hpp>
#include <kengine/core/assets/manager.hpp>
#include <kengine/core/fileio/file.hpp>

namespace kengine::core::graphics {

enum class ShaderMedium {
	Unknown = 0,
	Glsl = 1,
	Hlsl = 2,
	Msl = 3,
	SpirV = 4,
	DxBC = 5,
};

class IShaderAsset : UUIDAsset<IShaderAsset> {
public:
	virtual ~IShaderAsset() = default;

	virtual ShaderMedium getMedium() const = 0;
	virtual const char* getSource() const = 0;
};

class ShaderGLSLAsset : public IShaderAsset {
public:
	ShaderGLSLAsset(const char* source) : _source(source) {}
	ShaderGLSLAsset(std::string const& source) : _source(source) {}

	bool load(std::string const& path) override {
		kengine::core::fileio::File<char> file;
		if (!file.load(path)) {
			return false;
		}

		_source = std::string(file.getData(), file.getBytesize());
	}

	void unload() override {
		_source.clear();
	}

	bool isLoaded() override {
		return !_source.empty();
	}

	ShaderMedium getMedium() const override { return ShaderMedium::Glsl; }
	const char* getSource() const override { return _source; }

private:
	std::string _source;
};

} // namespace kengine::core::graphics

#endif