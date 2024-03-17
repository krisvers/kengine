#include <kengine/kengine.hpp>
#include <kengine/window.hpp>
#include <kengine/logger.hpp>
#include <kengine/input/input.hpp>
#include <kengine/graphics/opengl.hpp>
#include <kengine/util/time.hpp>

namespace kengine {

KEngine::KEngine() : m_window(800, 600) {
	renderer = graphics::createOpenGLRenderer();
}

KEngine::KEngine(graphics::IRenderer* rend) : m_window(800, 600) {
	renderer = rend;
}

void KEngine::setGame(GameInitFunc init, GameUpdateFunc update, GameDestroyFunc destroy) {
	m_gameInit = init;
	m_gameUpdate = update;
	m_gameDestroy = destroy;
}

int KEngine::main() {
	KEngine::window = &m_window;
	util::time::init();
	m_window.show();
	renderer->init();
	input::input::init();

	if (m_gameInit != nullptr && m_gameInit() != 0) {
		return -1;
	} else {
		while (!m_window.m_closed) {
			renderer->render();
			m_window.swapBuffers();
			if (m_window.update() != 0) {
				break;
			}

			input::input::update();

			if (m_gameUpdate != nullptr && m_gameUpdate() != 0) {
				break;
			}
		}
	}

	if (m_gameDestroy != nullptr) {
		m_gameDestroy();
	}

	input::input::destroy();

	renderer->destroy();
	return 0;
}

Window* KEngine::getWindow() {
	return KEngine::window;
}

} // namespace kengine
