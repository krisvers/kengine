#ifndef KENGINE_CORE_WINDOW_WINDOW_HPP
#define KENGINE_CORE_WINDOW_WINDOW_HPP

#include <string>

#include <kengine/types.hpp>
#include <kengine/singleton.hpp>

namespace kengine::core::window {

class IWindow;

class Window : public kengine::Singleton<Window> {
public:
	IWindow& create(std::string const& title);
	IWindow& create(kengine::u32 width, kengine::u32 height);
	IWindow& create(std::string const& title, kengine::u32 width, kengine::u32 height);

	void destroy(IWindow& window);
};

class IWindow {
protected:
	IWindow() = default;
	virtual ~IWindow() = default;

public:
	virtual void show() = 0;
	virtual void hide() = 0;

	virtual void setTitle(std::string const& title) = 0;
	virtual void setSize(kengine::u32 width, kengine::u32 height) = 0;
	virtual void setPosition(kengine::s32 x, kengine::s32 y) = 0;

	virtual bool isClosed() const = 0;

	virtual std::string const& getTitle() const = 0;
	virtual kengine::u32 getWidth() const = 0;
	virtual kengine::u32 getHeight() const = 0;
	virtual kengine::s32 getX() const = 0;
	virtual kengine::s32 getY() const = 0;
};

} // namespace kengine::core::window

#endif
