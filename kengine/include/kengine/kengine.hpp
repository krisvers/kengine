#ifndef KRISVERS_KENGINE_HPP
#define KRISVERS_KENGINE_HPP

#include <kengine/graphics/renderer.hpp>
#include <kengine/types.hpp>
#include <kengine/window.hpp>

namespace kengine {

using GameInitFunc = int (*)();
using GameUpdateFunc = int (*)();
using GameDestroyFunc = void (*)();

class KEngine {
public:
	graphics::IRenderer* renderer;

	KEngine();
	KEngine(graphics::IRenderer* renderer);

	void setGame(GameInitFunc init, GameUpdateFunc update, GameDestroyFunc destroy);
	int main();

	static Window* getWindow();

private:
	Window m_window;
	inline static Window* window;

	GameInitFunc m_gameInit;
	GameUpdateFunc m_gameUpdate;
	GameDestroyFunc m_gameDestroy;
};

} // namespace kengine

#endif
