#ifndef KENGINE_CORE_TRANSFORM_HPP
#define KENGINE_CORE_TRANSFORM_HPP

#include <kengine/types.hpp>
#include <kengine/util/math/vector.hpp>

namespace kengine::core {

struct Transform {
	Transform& operator=(Transform const& other) {
		position = other.position;
		rotation = other.rotation;
		scale = other.scale;
		return *this;
	}

	util::math::Vector<f32, 3> position = { 0.0f, 0.0f, 0.0f };
	util::math::Vector<f32, 3> rotation = { 0.0f, 0.0f, 0.0f };
	util::math::Vector<f32, 3> scale = { 1.0f, 1.0f, 1.0f };
};

} // namespace kengine::core

#endif