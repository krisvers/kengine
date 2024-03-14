#ifndef KRISVERS_KENGINE_GRAPHICS_OPENGL_HPP
#define KRISVERS_KENGINE_GRAPHICS_OPENGL_HPP

#include <kengine/graphics/renderer.hpp>
#include <kengine/types.hpp>

namespace kengine::graphics {

IRenderer* createOpenGLRenderer();
void destroyOpenGLRenderer(IRenderer* renderer);

} // namespace kengine::graphics

#endif
