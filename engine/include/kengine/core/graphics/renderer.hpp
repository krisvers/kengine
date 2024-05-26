#ifndef KENGINE_CORE_GRAPHICS_RENDERER_HPP
#define KENGINE_CORE_GRAPHICS_RENDERER_HPP

#include <stdexcept>

#include <kengine/singleton.hpp>
#include <kengine/core/exception.hpp>

namespace kengine::core::graphics {

class RendererException : Exception {
public:
	RendererException(std::string const& message) : Exception(message) {}
	RendererException(const char* message) : Exception(message) {}
};

class IRenderer {
protected:
	IRenderer() = default;
	virtual ~IRenderer() = default;

public:
	virtual void render() = 0;
};

class Renderer : public Singleton<Renderer> {
public:
	IRenderer* create();
	void destroy(IRenderer* renderer);
};

} // namespace kengine::core

#endif