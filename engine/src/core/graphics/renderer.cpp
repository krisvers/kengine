#include <kengine/core/graphics/renderer.hpp>
#include "dx10/renderer_dx10.hpp"

namespace kengine::core::graphics {

IRenderer* IRenderer::create() {
	return new dx10::RendererDX10();
}

void IRenderer::destroy(IRenderer* renderer) {
	delete (dx10::RendererDX10*) renderer;
}

} // namespace kengine::core