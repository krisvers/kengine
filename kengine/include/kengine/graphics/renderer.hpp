#ifndef KRISVERS_KENGINE_GRAPHICS_RENDERER_HPP
#define KRISVERS_KENGINE_GRAPHICS_RENDERER_HPP

#include <kengine/types.hpp>

namespace kengine::graphics {

class IRenderer {
public:
	virtual void init() = 0;
	virtual void render() = 0;
	virtual void destroy() = 0;
};

} // namespace kengine::graphics

#endif
