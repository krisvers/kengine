#ifndef KENGINE_CORE_GRAPHICS_DX10_RENDERER_HPP
#define KENGINE_CORE_GRAPHICS_DX10_RENDERER_HPP

#include <kengine/macros.hpp>

#ifdef KENGINE_PLATFORM_WINDOWS

#include <kengine/core/graphics/renderer.hpp>

namespace kengine::core::graphics::dx10 {

class RendererExceptionDX10 : public RendererException {
public:
	RendererExceptionDX10(std::string const& message) : RendererException(std::string("(DX10 exception) ") + message) {}
	RendererExceptionDX10(const char* message) : RendererException(std::string("(DX10 exception) ") + message) {}
};

class RendererDX10 : public IRenderer {
public:
	RendererDX10();
	~RendererDX10() override;

	void render() override;

private:
	class Implementation;
	Implementation* impl;
};

} // namespace kengine::core::graphics::dx10

#endif // KENGINE_PLATFORM_WINDOWS

#endif
