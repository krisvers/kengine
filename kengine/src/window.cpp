#include <kengine/window.hpp>
#include <kengine/logger.hpp>
#include <kengine/input/input.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#define WIN32_LEAN_AND_MEAN
#include <GLFW/glfw3native.h>

#ifdef ERROR
#undef ERROR
#endif

#include <exception>
#include <stdexcept>
#include <iostream>

#define GLFW_INIT() if (!glfwInited) { glfwSetErrorCallback(WindowCallbacks::glfwErrorCallback); if (!glfwInit()) { throw std::runtime_error("Failed to init GLFW"); } }
#define INTERNAL reinterpret_cast<GLFWwindow*>(m_internal)

namespace kengine {

static bool glfwInited = false;

struct WindowCallbacks {
	static void glfwErrorCallback(int err, const char* msg);
	static void glfwWindowCloseCallback(GLFWwindow* window);
	static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int mods, int action);
	static void glfwWindowSizeCallback(GLFWwindow* window, int width, int height);
};

static constexpr input::InputKey glfwKeyToInputKey(int key) {
	if (key >= 340) {
		return static_cast<input::InputKey>((key - 340) + static_cast<u32>(input::InputKey::KEY_LSHIFT));
	}
	if (key >= 320) {
		return static_cast<input::InputKey>((key - 320) + static_cast<u32>(input::InputKey::KEY_NUMPAD_0));
	}
	if (key >= 290) {
		return static_cast<input::InputKey>((key - 290) + static_cast<u32>(input::InputKey::KEY_F1));
	}
	if (key >= 280) {
		return static_cast<input::InputKey>((key - 280) + static_cast<u32>(input::InputKey::KEY_CAPS_LOCK));
	}
	if (key >= 256) {
		return static_cast<input::InputKey>((key - 256) + static_cast<u32>(input::InputKey::KEY_ESCAPE));
	}

	return static_cast<input::InputKey>(key);
}

Window::Window(u32 width, u32 height, const char* title) {
	GLFW_INIT();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	void* internal = reinterpret_cast<void*>(glfwCreateWindow(width, height, "Window", nullptr, nullptr));
	if (internal == nullptr) {
		throw std::runtime_error("Failed to create GLFW window");
	}

	m_width = width;
	m_height = height;

	m_internal = internal;
	glfwSetWindowUserPointer(INTERNAL, this);
	glfwSetWindowCloseCallback(INTERNAL, WindowCallbacks::glfwWindowCloseCallback);
	glfwSetKeyCallback(INTERNAL, WindowCallbacks::glfwKeyCallback);
	glfwSetWindowSizeCallback(INTERNAL, WindowCallbacks::glfwWindowSizeCallback);

	glfwMakeContextCurrent(INTERNAL);
}

void Window::hide() {
	glfwHideWindow(INTERNAL);
}

void Window::show() {
	glfwShowWindow(INTERNAL);
}

int Window::update() {
	glfwPollEvents();
	if (m_closed) {
		return 1;
	}

	return 0;
}

void Window::swapBuffers() {
	glfwSwapBuffers(INTERNAL);
}

void* Window::getHandleWin32() {
	return glfwGetWin32Window(INTERNAL);
}

void Window::inputPipeKey(input::InputKey key, bool value) {
	input::input::setKey(key, value);
}

void WindowCallbacks::glfwErrorCallback(int err, const char* msg) {
	logger::printf(kengine::LogType::ERROR, "GLFW error %x: %s", err, msg);
}

void WindowCallbacks::glfwWindowCloseCallback(GLFWwindow* window) {
	Window* w = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	w->m_closed = true;
}

void WindowCallbacks::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	input::InputKey ikey = glfwKeyToInputKey(key);
	Window::inputPipeKey(ikey, (action != 0));
}

void WindowCallbacks::glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {
	Window* w = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	w->m_width = width;
	w->m_height = height;
}

}
