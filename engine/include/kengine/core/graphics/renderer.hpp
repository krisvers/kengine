#ifndef KENGINE_CORE_GRAPHICS_RENDERER_HPP
#define KENGINE_CORE_GRAPHICS_RENDERER_HPP

#include <stdexcept>

#include <kengine/singleton.hpp>

namespace kengine::core::graphics {

class RendererException : public std::runtime_error {
public:
	RendererException(std::string const& message) : std::runtime_error(message) {}
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