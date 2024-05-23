#ifndef KENGINE_CORE_WINDOW_HPP
#define KENGINE_CORE_WINDOW_HPP

#include <string>
#include <kengine/types.hpp>

namespace kengine::core {

class Window {
public:
	Window(std::string const& title);
	Window(kengine::u32 width, kengine::u32 height);
	Window(std::string const& title, kengine::u32 width, kengine::u32 height);
	~Window();

	void show();
	void hide();

	void setTitle(std::string const& title);
	void setSize(kengine::u32 width, kengine::u32 height);
	void setPosition(kengine::s32 x, kengine::s32 y);

private:
	class Implementation;
	Implementation* impl;
};

} // namespace kengine::core

#endif