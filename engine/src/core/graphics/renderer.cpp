#include <kengine/core/graphics/renderer.hpp>
#include "dx10/renderer_dx10.hpp"
#include "gl41/renderer_gl41.hpp"

namespace kengine::core::graphics {

IRenderer* Renderer::create() {
#ifdef KENGINE_PLATFORM_WINDOWS
	return new dx10::RendererDX10();
#else
	return new gl41::RendererGL41();
#endif
}

void Renderer::destroy(IRenderer* renderer) {
#ifdef KENGINE_PLATFORM_WINDOWS
	delete dynamic_cast<dx10::RendererDX10*>(renderer);
#else
	delete dynamic_cast<gl41::RendererGL41*>(renderer);
#endif
}

} // namespace kengine::core
