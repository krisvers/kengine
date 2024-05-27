#ifndef KENGINE_CORE_GRAPHICS_RENDERER_HPP
#define KENGINE_CORE_GRAPHICS_RENDERER_HPP

#include <stdexcept>

#include <kengine/singleton.hpp>
#include <kengine/core/exception.hpp>
#include <kengine/core/window/window.hpp>

namespace kengine::core::graphics {

class RendererException : Exception {
public:
	RendererException(std::string const& message) : Exception(message) {}
	RendererException(const char* message) : Exception(message) {}
};

class IRenderer;

class Renderer : public Singleton<Renderer> {
public:
	IRenderer& create(window::IWindow& window);
	void destroy(IRenderer& renderer);
};

class IRenderer {
protected:
	IRenderer() = default;
	IRenderer(window::IWindow& window);
	virtual ~IRenderer() = default;

public:
	virtual void render() = 0;
};

} // namespace kengine::core

#endif