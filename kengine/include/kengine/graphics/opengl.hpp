#ifndef KRISVERS_KENGINE_GRAPHICS_OPENGL_HPP
#define KRISVERS_KENGINE_GRAPHICS_OPENGL_HPP

#include <kengine/graphics/renderer.hpp>
#include <kengine/types.hpp>

namespace kengine::graphics::opengl {

void getOpenGLSupportedRenderers(std::vector<RendererSupport>& renderers);
IRenderer* createOpenGLRenderer();
IRenderer* createOpenGLRendererWithSupport(RendererSupport renderer);
void destroyOpenGLRenderer(IRenderer* renderer);

} // namespace kengine::graphics::opengl

#endif
