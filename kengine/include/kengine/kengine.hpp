#ifndef KRISVERS_KENGINE_HPP
#define KRISVERS_KENGINE_HPP

#include <kengine/graphics/renderer.hpp>
#include <kengine/types.hpp>
#include <kengine/window.hpp>

namespace kengine {

class KEngine {
public:
	KEngine();
	KEngine(graphics::IRenderer* renderer);

	int main();

private:
	graphics::IRenderer* m_renderer;
	Window m_window;
};

} // namespace kengine

#endif
