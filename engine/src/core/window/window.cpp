#include <kengine/core/window/window.hpp>
#include <kengine/macros.hpp>
#include "xlib/window_xlib.hpp"
#include "win32/window_win32.hpp"
#include "cocoa/window_cocoa.hpp"

namespace kengine::core::window {

IWindow* IWindow::create(std::string const& title) {
	return create(title, 800, 600);
}

IWindow* IWindow::create(kengine::u32 width, kengine::u32 height) {
	return create("KEngine Window", width, height);
}

IWindow* IWindow::create(std::string const& title, kengine::u32 width, kengine::u32 height) {
#ifdef KENGINE_PLATFORM_WINDOWS
	return new win32::WindowWin32(title, width, height);
#elif defined(KENGINE_PLATFORM_LINUX)
	return xlib::WindowXlib::create(title, width, height);
#elif defined(KENGINE_PLATFORM_MACOS)
	return new cocoa::WindowCocoa(title, width, height);
#else
	return nullptr;
#endif
}

} // namespace kengine::core::window
