#ifndef KENGINE_CORE_WINDOW_XLIB_WINDOW_HPP
#define KENGINE_CORE_WINDOW_XLIB_WINDOW_HPP

#include <kengine/macros.hpp>

#ifdef KENGINE_PLATFORM_LINUX

#include <string>

#include <kengine/core/window/window.hpp>

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

	bool isClosed() const override;

	std::string const& getTitle() const override;
	kengine::u32 getWidth() const override;
	kengine::u32 getHeight() const override;
	kengine::s32 getX() const override;
	kengine::s32 getY() const override;
};

} // namespace kengine::core::window::xlib

#endif // KENGINE_PLATFORM_LINUX

#endif