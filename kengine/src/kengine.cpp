#include <kengine/kengine.hpp>
#include <kengine/graphics/opengl.hpp>

KEngine::KEngine() {
	m_renderer = new KRendererOpenGL();
}

KEngine::KEngine(KRenderer* renderer) {
	m_renderer = renderer;
}

int KEngine::main() {
	m_renderer->init();
	while (true) {
		m_renderer->draw();
	}
	m_renderer->destroy();
}
