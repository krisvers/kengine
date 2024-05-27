#include "renderer_gl41.hpp"

#include <kengine/core/graphics/renderer.hpp>
#include <stdexcept>

namespace kengine::core::graphics::gl41 {

struct Vertex {
	float x, y, z;
};

RendererGL41::RendererGL41(window::IWindow& window) : _window(window) {

}

RendererGL41::~RendererGL41() {

}

void RendererGL41::render() {

}

} // namespace kengine::core::graphics::gl41
