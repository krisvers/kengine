#include <kengine/kengine.hpp>
#include <kengine/graphics/opengl.hpp>

namespace kengine {

KEngine::KEngine() {
	m_renderer = new graphics::KRendererOpenGL();
}

KEngine::KEngine(graphics::KRenderer* renderer) {
	m_renderer = renderer;
}

int KEngine::main() {
	m_renderer->init();
	while (true) {
		m_renderer->draw();
	}
	m_renderer->destroy();
}

} // namespace kengine