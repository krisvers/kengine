#include <kengine/window.hpp>
#include <GLFW/glfw3.h>
#include <exception>
#include <stdexcept>
#include <iostream>

#define GLFW_INIT() if (!glfwInited) { if (!glfwInit()) { throw std::runtime_error("Failed to init GLFW"); } }
#define INTERNAL reinterpret_cast<GLFWwindow*>(m_internal)

static bool glfwInited = false;

static void glfwlogger(int err, const char* msg) {
	std::cout << msg << " err: " << err << "\n";
}

namespace kengine {

Window::Window(u32 width, u32 height, const char* title) {
	//GLFW_INIT();
	glfwInit();
	glfwSetErrorCallback(glfwlogger);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);

	void* internal = reinterpret_cast<void*>(glfwCreateWindow(width, height, "Window", nullptr, nullptr));
	if (internal == nullptr) {
		throw std::runtime_error("Failed to create GLFW window");
	}

	m_internal = internal;
	glfwSetWindowUserPointer(INTERNAL, this);
}

void Window::hide() {
	
}

}
