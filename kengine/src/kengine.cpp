#include <kengine/kengine.hpp>
#include <kengine/window.hpp>
#include <kengine/logger.hpp>
#include <kengine/input/input.hpp>
#include <kengine/graphics/opengl.hpp>

namespace kengine {

KEngine::KEngine() : m_window(800, 600) {
	m_renderer = graphics::createOpenGLRenderer();
}

KEngine::KEngine(graphics::IRenderer* renderer) : m_window(800, 600) {
	m_renderer = renderer;
}

int KEngine::main() {
	logger::printf(LogType::WARNING, "hello, world %s %f\n", "102", 69.420f);
	m_window.show();
	m_renderer->init();

	graphics::Mesh mesh = graphics::Mesh();
	mesh.vertices.push_back(
		graphics::Vertex(util::Vector<f32, 3>{ -1, -1, 0 })
	);
	mesh.vertices.push_back(
		graphics::Vertex(util::Vector<f32, 3>{ 1, -1, 0 })
	);
	mesh.vertices.push_back(
		graphics::Vertex(util::Vector<f32, 3>{ 0, 1, 0 })
	);

	graphics::Renderable* renderable = m_renderer->createRenderable();
	m_renderer->uploadRenderableMesh(renderable, &mesh);

	while (!m_window.m_closed) {
		m_renderer->render();
		m_window.swapBuffers();
		if (m_window.update() != 0) {
			break;
		}

		if (input::input::isKey(input::InputKey::KEY_SPACE)) {
			logger::print(LogType::DEBUG, "space\n");
		}
	}

	m_renderer->destroyRenderable(renderable);
	m_renderer->destroy();
	return 0;
}

} // namespace kengine
