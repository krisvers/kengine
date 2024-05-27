#ifndef KENGINE_CORE_GRAPHICS_GL41_RENDERER_HPP
#define KENGINE_CORE_GRAPHICS_GL41_RENDERER_HPP

#include <Windows.h>
#include <gl/GL.h>

#include <kengine/core/graphics/renderer.hpp>

namespace kengine::core::graphics::gl41 {

class RendererExceptionGL41 : public RendererException {
public:
	RendererExceptionGL41(std::string const& message) : RendererException("(DX10 exception) " + message) {}
};

class RendererGL41 : public IRenderer {
public:
	RendererGL41(window::IWindow& window);
	~RendererGL41();

	void render() override;

private:
	GLint _program = 0;
	GLint _vertexShader = 0;
	GLint _fragmentShader = 0;

	window::IWindow& _window;
};

} // namespace kengine::core::graphics::gl41

#endif
