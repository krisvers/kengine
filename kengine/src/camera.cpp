#include <kengine/camera.hpp>

#define M_PI 3.14159265358979323846

namespace kengine {

void Camera::calculateViewMatrix() {
	m_viewMatrix.identity().translate(-position[0], -position[1], -position[2]).rotate(-rotation[0] * M_PI / 180.0f, -rotation[1] * M_PI / 180.0f, rotation[2] * M_PI / 180.0f);
}

void Camera::calculateProjectionMatrix() {
	if (isOrthographic) {
		m_projectionMatrix.orthographic(-size[0] * aspect, size[0] * aspect, -size[1], size[1], near, far);
	} else {
		m_projectionMatrix.perspective(fov * M_PI / 180.0f, aspect, near, far);
	}
}

} // namespace kengine