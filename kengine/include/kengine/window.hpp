#ifndef KRISVERS_KENGINE_WINDOW_HPP
#define KRISVERS_KENGINE_WINDOW_HPP

#include <kengine/types.hpp>
#include <kengine/input/key.hpp>

namespace kengine {

struct WindowCallbacks;

class Window {
public:
	u32 width;
	u32 height;
	bool closed;

	Window(u32 width, u32 height) : Window(width, height, "Window") {};
	Window(u32 width, u32 height, const char* title);

	void hide();
	void show();

	int update();
	void swapBuffers();

	void* getHandleWin32();

	friend struct WindowCallbacks;

private:
	static void inputPipeKey(input::InputKey key, bool value);

	void* m_internal;
};

} // namespace kengine

#endif
