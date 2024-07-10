#ifndef KENGINE_CORE_WINDOW_WINDOW_WIN_HPP
#define KENGINE_CORE_WINDOW_WINDOW_WIN_HPP

#include <kengine/macros.hpp>

#ifdef KENGINE_PLATFORM_WINDOWS

#include <kengine/core/window/window.hpp>
#include <Windows.h>

namespace kengine::core::window::win {

class WindowWin : public IWindow {
public:
	WindowWin(std::string const& title, kengine::u32 width, kengine::u32 height);
	~WindowWin() override;

	void show() override;
	void hide() override;

	void setTitle(std::string const& title) override;
	void setSize(kengine::u32 width, kengine::u32 height) override;
	void setPosition(kengine::s32 x, kengine::s32 y) override;

	bool isClosed() const override { return _closed; }

	std::string const& getTitle() const override { return _title; }
	kengine::u32 getWidth() const override { return _width; }
	kengine::u32 getHeight() const override { return _height; }
	kengine::s32 getX() const override { return _x; }
	kengine::s32 getY() const override { return _y; }

	HWND getHwnd() const { return _hwnd; }
	HDC getHdc() const { return GetDC(_hwnd); }

private:
	inline static LRESULT CALLBACK hwndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	HWND _hwnd = nullptr;
	WNDCLASSA _wc = {};
	std::string _title = "";
	kengine::u32 _width, _height = 0;
	kengine::s32 _x, _y = 0;
	bool _closed = false;
};

} // namespace kengine::core::window::win

#endif // KENGINE_PLATFORM_WINDOWS

#endif