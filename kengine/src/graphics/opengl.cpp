#include <kengine/graphics/opengl.hpp>

#include <glad/glad.h>

#include <iostream>
#include <vector>

namespace kengine::graphics {

struct RendererOpenGLBackend {
	int init();
	int draw();
	void destroy();
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

int RendererOpenGLBackend::draw() {
	
}

void RendererOpenGLBackend::destroy() {

}

} // namespace kengine::graphics