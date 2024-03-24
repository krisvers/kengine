#ifndef KRISVERS_KENGINE_GRAPHICS_D3D10_HPP
#define KRISVERS_KENGINE_GRAPHICS_D3D10_HPP

#include <kengine/graphics/renderer.hpp>
#include <kengine/types.hpp>

namespace kengine::graphics::d3d10 {

void getD3D10SupportedRenderers(std::vector<RendererSupport>& renderers);
IRenderer* createD3D10Renderer();
IRenderer* createD3D10RendererWithSupport(RendererSupport renderer);
void destroyD3D10Renderer(IRenderer* renderer);

} // namespace kengine::graphics::d3d10

#endif
