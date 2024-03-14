#ifndef KRISVERS_KENGINE_GRAPHICS_RENDERABLE_HPP
#define KRISVERS_KENGINE_GRAPHICS_RENDERABLE_HPP

#include <kengine/graphics/material.hpp>

namespace kengine::graphics {

class IRenderable {
public:
	Material albedo;

private:
	void* internal;
};

} // namespace kegnine::graphics

#endif