#include "renderer_gl41.hpp"

#include <kengine/core/graphics/renderer.hpp>
#include <stdexcept>

namespace kengine::core::graphics::gl41 {

struct Vertex {
	float x, y, z;
};

class RendererGL41::Implementation {
private:

public:
	Implementation() {
	}

	~Implementation() {
	}

	void render() {
	}
};

RendererGL41::RendererGL41() {
	impl = new RendererGL41::Implementation();
}

RendererGL41::~RendererGL41() {
	delete impl;
}

void RendererGL41::render() {
	impl->render();
}

} // namespace kengine::core::graphics::gl41
