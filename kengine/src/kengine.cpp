#include <kengine/kengine.hpp>
#include <kengine/window.hpp>
#include <kengine/logger.hpp>
#include <kengine/input/input.hpp>
#include <kengine/graphics/opengl.hpp>

namespace kengine {

KEngine::KEngine() : m_window(800, 600) {
	m_renderer = new graphics::RendererOpenGL();
}

KEngine::KEngine(graphics::IRenderer* renderer) : m_window(800, 600) {
	m_renderer = renderer;
}

int KEngine::main() {
	logger::printf(LogType::WARNING, "hello, world %s %f\n", "102", 69.420f);
	m_window.show();
	m_renderer->init();

	while (!m_window.m_closed) {
		m_renderer->render();
		m_window.swapBuffers();
		if (m_window.update() != 0) {
			break;
		}

		logger::printf(LogType::DEBUG, "(%u, %u)\n", m_window.m_width, m_window.m_height);

		if (input::input::isKey(input::InputKey::KEY_SPACE)) {
			logger::print(LogType::DEBUG, "space\n");
		}
	}
	m_renderer->destroy();
	return 0;
}

} // namespace kengine
