#include <kengine/core/graphics/renderer.hpp>
#include "gl41/renderer_gl41.hpp"

namespace kengine::core::graphics {

IRenderer& Renderer::create(window::IWindow& window) {
	return *new gl41::RendererGL41(window);
}

void Renderer::destroy(IRenderer& renderer) {
	delete dynamic_cast<gl41::RendererGL41*>(&renderer);
}

} // namespace kengine::core
