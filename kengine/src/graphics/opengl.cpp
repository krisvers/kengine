#include <kengine/graphics/opengl.hpp>
#include <iostream>
#include <glad/glad.h>

namespace kengine::graphics {

struct RendererOpenGLBackend {
	int init();
};

void RendererOpenGL::init() {
	m_backend = new RendererOpenGLBackend();
	if (m_backend->init() != 0) {
		throw std::runtime_error("Failed to initialize OpenGL renderer backend\n");
	}
}

void RendererOpenGL::render() {
	//std::cout << "opengl draw\n";
}

void RendererOpenGL::destroy() {
	std::cout << "opengl destroy\n";
}

int RendererOpenGLBackend::init() {
	if (!gladLoadGL()) {
		std::cerr << "Failed to initialize OpenGL context\n";
		return 1;
	}

	return 0;
}

} // namespace kengine::graphics
