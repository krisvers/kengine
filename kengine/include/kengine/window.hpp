#ifndef KRISVERS_KENGINE_WINDOW_HPP
#define KRISVERS_KENGINE_WINDOW_HPP

#include <kengine/types.hpp>

namespace kengine {

class Window {
public:
	u32 m_width;
	u32 m_height;

	Window(u32 width, u32 height) : Window(width, height, "Window") {};
	Window(u32 width, u32 height, const char* title);

	void hide();
	void show();

private:
	void* m_internal;
};

} // namespace kengine

#endif
