#include <kengine/graphics/opengl.hpp>
#include <iostream>

namespace kengine::graphics {

void KRendererOpenGL::init() {
	std::cout << "opengl init\n";
}

void KRendererOpenGL::draw() {
	std::cout << "opengl draw\n";
}

void KRendererOpenGL::destroy() {
	std::cout << "opengl destroy\n";
}

} // namespace kengine::graphics