#ifndef KRISVERS_KENGINE_GRAPHICS_OPENGL_HPP
#define KRISVERS_KENGINE_GRAPHICS_OPENGL_HPP

#include <kengine/graphics/renderer.hpp>

namespace kengine::graphics {

class KRendererOpenGL : public KRenderer {
	virtual void init();
	virtual void draw();
	virtual void destroy();
};

} // namespace kengine::graphics

#endif
