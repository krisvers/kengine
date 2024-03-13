#include <kengine/graphics/opengl.hpp>
#include <iostream>
#include <glad/glad.h>

namespace kengine::graphics {

struct RendererOpenGLBackend {

};

void RendererOpenGL::init() {
	std::cout << "opengl init\n";
}

void RendererOpenGL::render() {
	//std::cout << "opengl draw\n";
}

void RendererOpenGL::destroy() {
	std::cout << "opengl destroy\n";
}

} // namespace kengine::graphics
