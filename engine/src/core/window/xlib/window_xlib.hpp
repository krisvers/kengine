#ifndef KENGINE_CORE_WINDOW_XLIB_WINDOW_HPP
#define KENGINE_CORE_WINDOW_XLIB_WINDOW_HPP

#include <kengine/core/window/window.hpp>
#include <string>

namespace kengine::core::window::xlib {

class WindowXlib : public IWindow {
public:
	WindowXlib(std::string const& title, kengine::u32 width, kengine::u32 height);
	~WindowXlib() override;

	void show() override;
	void hide() override;

	void setTitle(std::string const& title) override;
	void setSize(kengine::u32 width, kengine::u32 height) override;
	void setPosition(kengine::s32 x, kengine::s32 y) override;

	static WindowXlib* create(std::string const& title, kengine::u32 width, kengine::u32 height);
};

} // namespace kengine::core::window::xlib

#endif

