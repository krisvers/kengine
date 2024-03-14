#ifndef KRISVERS_KENGINE_GRAPHICS_RENDERER_HPP
#define KRISVERS_KENGINE_GRAPHICS_RENDERER_HPP

#include <kengine/types.hpp>
#include <kengine/graphics/renderable.hpp>

#include <vector>

namespace kengine::graphics {

class IRenderer {
public:
	virtual void init() = 0;
	virtual void render() = 0;
	virtual void destroy() = 0;

private:
	std::vector<IRenderable*> m_renderables;
};

} // namespace kengine::graphics

#endif
