#include <kengine/core/kengine.hpp>
#include <kengine/core/graphics/renderer.hpp>
#include <kengine/core/logging.hpp>

namespace kengine::core {

void KEngine::run() {
	Logger::logf("hello world");
	graphics::IRenderer* renderer = graphics::IRenderer::create();

	while (true) {
		renderer->render();
	}
}

} // namespace kengine::core