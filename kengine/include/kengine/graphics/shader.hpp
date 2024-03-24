#ifndef KRISVERS_KENGINE_GRAPHICS_SHADER_HPP
#define KRISVERS_KENGINE_GRAPHICS_SHADER_HPP

namespace kengine::graphics {

enum class ShaderType {
	Postprocess
};

enum class ShaderMedium {
	GLSL,
	HLSL,
	DXBC,
	Metal,
	SpirV,
	Unknown
};

struct ShaderVersion {
	u8 major;
	u8 minor;
	u8 patch;
};

struct ShaderDescriptor {
	ShaderType type;
	ShaderMedium medium;
	ShaderVersion version;
};

} // namespace kengine::graphics

#endif