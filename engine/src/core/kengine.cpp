#include <kengine/core/kengine.hpp>
#include <kengine/core/graphics/renderer.hpp>
#include <kengine/core/logging.hpp>

namespace kengine::core {

void KEngine::run() {
	Logger::logf(LogSeverity::VERBOSE, "hello world {}", "\"this is the zaza man speaking\"");
	graphics::IRenderer* renderer = graphics::IRenderer::create();

	while (true) {
		renderer->render();
	}
}

} // namespace kengine::core
