#include <kengine/kengine.hpp>
#include <kengine/window.hpp>
#include <kengine/graphics/opengl.hpp>

namespace kengine {

KEngine::KEngine() : m_window(800, 600) {
	m_renderer = new graphics::RendererOpenGL();
}

KEngine::KEngine(graphics::IRenderer* renderer) : m_window(800, 600) {
	m_renderer = renderer;
}

int KEngine::main() {
	m_renderer->init();
	while (true) {
		m_renderer->render();
	}
	m_renderer->destroy();
}

} // namespace kengine
