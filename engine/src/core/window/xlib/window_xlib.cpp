#include "window_xlib.hpp"

#ifdef KENGINE_PLATFORM_LINUX

namespace kengine::core::window::xlib {

WindowXlib::WindowXlib(std::string const& title, kengine::u32 width, kengine::u32 height) {
}

WindowXlib::~WindowXlib() {

}

void WindowXlib::show() {

}

void WindowXlib::hide() {

}

void WindowXlib::setTitle(std::string const& title) {

}

void WindowXlib::setSize(kengine::u32 width, kengine::u32 height) {

}

void WindowXlib::setPosition(kengine::s32 x, kengine::s32 y) {

}

bool WindowXlib::isClosed() const {
	return false;
}

std::string const& WindowXlib::getTitle() const {
	static std::string s = "[to be implemented]";
	return s;
}

kengine::u32 WindowXlib::getWidth() const {
	return 100;
}

kengine::u32 WindowXlib::getHeight() const {
	return 100;
}

kengine::s32 WindowXlib::getX() const {
	return 0;
}

kengine::s32 WindowXlib::getY() const {
	return 0;
}

} // namespace kengine::core::window::xlib

#endif // KENGINE_PLATFORM_LINUX