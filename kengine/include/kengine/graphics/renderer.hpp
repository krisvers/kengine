#ifndef KRISVERS_KENGINE_GRAPHICS_RENDERER_HPP
#define KRISVERS_KENGINE_GRAPHICS_RENDERER_HPP

class KRenderer;

#include <kengine/kengine.hpp>

class KRenderer {
public:
	virtual void init() = 0;
	virtual void draw() = 0;
	virtual void destroy() = 0;
};

#endif
