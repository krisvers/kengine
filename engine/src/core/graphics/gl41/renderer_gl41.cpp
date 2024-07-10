#include "renderer_gl41.hpp"

#include <stdexcept>

#include <kengine/core/graphics/renderer.hpp>
#include <kengine/core/exception.hpp>

#include "../../window/win/window_win.hpp"
#include "../../window/xlib/window_xlib.hpp"
#include "../../window/cocoa/window_cocoa.hpp"

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC, HGLRC, const int*);

namespace kengine::core::graphics::gl41 {

struct Vertex {
	float x, y, z;
};

RendererGL41::RendererGL41(window::IWindow& window) : _window(window) {
	_initGL();

	_intermediateFramebuffers[0].startSetup()
		.setupTexture(0, _window.getWidth(), _window.getHeight(), GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, GL_COLOR_ATTACHMENT0)
		.setupTexture(1, _window.getWidth(), _window.getHeight(), GL_RGBA, GL_RGBA16F, GL_FLOAT, GL_NEAREST, GL_COLOR_ATTACHMENT1)
		.setupTexture(2, _window.getWidth(), _window.getHeight(), GL_RGBA, GL_RGBA16F, GL_FLOAT, GL_NEAREST, GL_COLOR_ATTACHMENT2)
		.endSetup();

	_intermediateFramebuffers[1].startSetup()
		.setupTexture(0, _window.getWidth(), _window.getHeight(), GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, GL_COLOR_ATTACHMENT0)
		.setupTexture(1, _window.getWidth(), _window.getHeight(), GL_RGBA, GL_RGBA16F, GL_FLOAT, GL_NEAREST, GL_COLOR_ATTACHMENT1)
		.setupTexture(2, _window.getWidth(), _window.getHeight(), GL_RGBA, GL_RGBA16F, GL_FLOAT, GL_NEAREST, GL_COLOR_ATTACHMENT2)
		.endSetup();

	const char* vertexShaderSource = R"(
#version 410 core

out vec2 v_uv;

const vec2 vertices[6] = vec2[](
	vec2(-1.0, -1.0),
	vec2( 1.0, -1.0),
	vec2( 1.0,  1.0),
	vec2(-1.0, -1.0),
	vec2(-1.0,  1.0),
	vec2( 1.0,  1.0)
);

void main() {
	v_uv = (vertices[gl_VertexID] + 1.0) / 2.0;
	gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
}
	)";

	const char* fragmentShaderSource = R"(
#version 410 core

uniform sampler2D u_colorSampler;
uniform sampler2D u_positionSampler;
uniform sampler2D u_normalSampler;

in vec2 v_uv;
out vec4 o_color;

void main() {
	o_color = texture(u_colorSampler, v_uv);
}
	)";

	if (!_litShader.compile(vertexShaderSource, fragmentShaderSource)) {
		throw Exception("Failed to compile lit shader");
	}
}

RendererGL41::~RendererGL41() {
	_deinitGL();
}

void RendererGL41::_initGL() {
#ifdef KENGINE_PLATFORM_WINDOWS
	_hdc = static_cast<window::win::WindowWin&>(_window).getHdc();

	PIXELFORMATDESCRIPTOR pfd = { 0 };
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pixelFormat = ChoosePixelFormat(_hdc, &pfd);
	SetPixelFormat(_hdc, pixelFormat, &pfd);

	_hglrc = wglCreateContext(_hdc);
	if (_hglrc == nullptr) {
		throw Exception("Failed to create OpenGL context");
	}

	if (!wglMakeCurrent(_hdc, _hglrc)) {
		throw Exception("Failed to make OpenGL context current");
	}

	if (!gladLoadGL()) {
		throw Exception("Failed to load OpenGL functions");
	}

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, 0,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
	if (wglCreateContextAttribsARB == nullptr) {
		throw Exception("Failed to get wglCreateContextAttribsARB function");
	}

	HGLRC hglrc = wglCreateContextAttribsARB(_hdc, nullptr, attribs);
	if (hglrc == nullptr) {
		throw Exception("Failed to create OpenGL 4.1 context");
	}

	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(_hglrc);
	_hglrc = hglrc;

	if (!wglMakeCurrent(_hdc, _hglrc)) {
		throw Exception("Failed to make OpenGL 4.1 context current");
	}
#endif // KENGINE_PLATFORM_WINDOWS
}

void RendererGL41::_deinitGL() {
#ifdef KENGINE_PLATFORM_WINDOWS
	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(_hglrc);
#endif // KENGINE_PLATFORM_WINDOWS
}

void RendererGL41::_swapBuffers() {
#ifdef KENGINE_PLATFORM_WINDOWS
	SwapBuffers(static_cast<window::win::WindowWin&>(_window).getHdc());
#endif
}

void RendererGL41::render() {
	_intermediateFramebuffers[0].bindForDrawing();

	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, 800, 600);

	_intermediateFramebuffers[0].bindForReading();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer(0, 0, 800, 600, 0, 0, 800, 600, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	_swapBuffers();
}

} // namespace kengine::core::graphics::gl41
