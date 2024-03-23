#ifndef KRISVERS_KENGINE_GRAPHICS_RENDERABLE_HPP
#define KRISVERS_KENGINE_GRAPHICS_RENDERABLE_HPP

#include <kengine/graphics/material.hpp>
#include <kengine/math/matrix.hpp>

namespace kengine::graphics {

class Renderable {
public:
	Material albedo;
	math::Matrix<4, 4> modelMatrix;
};

} // namespace kegnine::graphics

#endif
