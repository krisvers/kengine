#ifndef KRISVERS_KENGINE_CAMERA_HPP
#define KRISVERS_KENGINE_CAMERA_HPP

#include <kengine/types.hpp>
#include <kengine/math/matrix.hpp>
#include <kengine/math/vector.hpp>

namespace kengine {

struct Camera {
	Camera() : position({0, 0, 0}), rotation({0, 0, 0}), fov(60), aspect(1), near(0.0f), far(100.0f), size({1, 1}), isOrthographic(true) {}

	math::Vector<f32, 3> position;
	math::Vector<f32, 3> rotation;

	/* perspective-projection members */
	f32 fov;
	f32 aspect;
	f32 near;
	f32 far;

	/* orthographic-projection members */
	math::Vector<f32, 2> size;

	bool isOrthographic;

	void calculateViewMatrix();
	void calculateProjectionMatrix();

	math::Matrix<4, 4> getViewMatrix() { return m_viewMatrix; }
	math::Matrix<4, 4> getProjectionMatrix() { return m_projectionMatrix; }

private:
	math::Matrix<4, 4> m_viewMatrix;
	math::Matrix<4, 4> m_projectionMatrix;
};

}

#endif
