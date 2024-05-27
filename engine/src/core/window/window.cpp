#include <kengine/core/window/window.hpp>
#include <kengine/macros.hpp>
#include "xlib/window_xlib.hpp"
#include "win/window_win.hpp"
#include "cocoa/window_cocoa.hpp"

namespace kengine::core::window {

IWindow& Window::create(std::string const& title) {
	return create(title, 800, 600);
}

IWindow& Window::create(kengine::u32 width, kengine::u32 height) {
	return create("KEngine Window", width, height);
}

IWindow& Window::create(std::string const& title, kengine::u32 width, kengine::u32 height) {
#ifdef KENGINE_PLATFORM_WINDOWS
	return *new win::WindowWin(title, width, height);
#elif defined(KENGINE_PLATFORM_LINUX)
	return *new xlib::WindowXlib(title, width, height);
#elif defined(KENGINE_PLATFORM_MACOS)
	return *new cocoa::WindowCocoa(title, width, height);
#endif
}

void Window::destroy(IWindow& window) {
#ifdef KENGINE_PLATFORM_WINDOWS
	delete static_cast<win::WindowWin*>(&window);
#elif defined(KENGINE_PLATFORM_LINUX)
	delete static_cast<xlib::WindowXlib*>(&window);
#elif defined(KENGINE_PLATFORM_MACOS)
	delete static_cast<cocoa::WindowCocoa*>(&window);
#endif
}

} // namespace kengine::core::window
