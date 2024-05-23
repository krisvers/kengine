#ifndef KENGINE_CORE_WINDOW_WINDOW_HPP
#define KENGINE_CORE_WINDOW_WINDOW_HPP

#include <string>
#include <kengine/types.hpp>

namespace kengine::core::window {

class IWindow {
protected:
	IWindow() = default;

public:
	virtual ~IWindow() = default;

	virtual void show() = 0;
	virtual void hide() = 0;

	virtual void setTitle(std::string const& title) = 0;
	virtual void setSize(kengine::u32 width, kengine::u32 height) = 0;
	virtual void setPosition(kengine::s32 x, kengine::s32 y) = 0;

	static IWindow* create(std::string const& title);
	static IWindow* create(kengine::u32 width, kengine::u32 height);
	static IWindow* create(std::string const& title, kengine::u32 width, kengine::u32 height);

	static void destroy(IWindow* window);
};

} // namespace kengine::core::window

#endif
