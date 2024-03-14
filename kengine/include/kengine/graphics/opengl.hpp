#ifndef KRISVERS_KENGINE_GRAPHICS_OPENGL_HPP
#define KRISVERS_KENGINE_GRAPHICS_OPENGL_HPP

#include <kengine/graphics/renderer.hpp>
#include <kengine/types.hpp>

namespace kengine::graphics {

struct RendererOpenGLBackend;

class RendererOpenGL : public IRenderer {
public:
	virtual void init();
	virtual void render();
	virtual void destroy();

private:
	struct RendererOpenGLBackend* m_backend;
};

} // namespace kengine::graphics

#endif
