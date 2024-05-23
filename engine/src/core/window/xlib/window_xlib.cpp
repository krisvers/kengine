#include "window_xlib.hpp"

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

WindowXlib* WindowXlib::create(std::string const& title, kengine::u32 width, kengine::u32 height) {
	return new WindowXlib(title, width, height);
}

}
