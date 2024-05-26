#include "window_win.hpp"

#ifdef KENGINE_PLATFORM_WINDOWS

#include <kengine/core/logging.hpp>

namespace kengine::core::window::win {

WindowWin::WindowWin(std::string const& title, kengine::u32 width, kengine::u32 height)
	: _title(title), _width(width), _height(height)
{
	_wc = {};
	_wc.lpfnWndProc = hwndProc;
	_wc.hInstance = GetModuleHandleA(nullptr);
	_wc.lpszClassName = "KEngineWindow";
	RegisterClassA(&_wc);

	RECT rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	_hwnd = CreateWindowA(_wc.lpszClassName, title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, _wc.hInstance, nullptr);
	if (_hwnd == nullptr) {
		kengine::core::Logger::get().log(kengine::core::LogSeverity::Fatal, "Failed to create a window");
		throw std::runtime_error("Failed to create window");
	}

	SetWindowLongPtrA(_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	HMONITOR monitor = MonitorFromWindow(_hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
	GetMonitorInfoA(monitor, &monitorInfo);

	setPosition(monitorInfo.rcMonitor.left + (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left - width) / 2,
		monitorInfo.rcMonitor.top + (monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top - height) / 2);
}

WindowWin::~WindowWin() {
	DestroyWindow(_hwnd);
	UnregisterClassA(_wc.lpszClassName, _wc.hInstance);
}

void WindowWin::show() {
	ShowWindow(_hwnd, SW_SHOW);
}

void WindowWin::hide() {
	ShowWindow(_hwnd, SW_HIDE);
}

void WindowWin::setTitle(std::string const& title) {
	_title = title;
	SetWindowTextA(_hwnd, title.c_str());
}

void WindowWin::setSize(kengine::u32 width, kengine::u32 height) {
	_width = width;
	_height = height;
	SetWindowPos(_hwnd, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

void WindowWin::setPosition(kengine::s32 x, kengine::s32 y) {
	_x = x;
	_y = y;
	SetWindowPos(_hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

LRESULT CALLBACK WindowWin::hwndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	WindowWin* self = reinterpret_cast<WindowWin*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));

	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		self->_closed = true;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		self->_closed = true;
		break;
	case WM_SIZE:
		self->_width = LOWORD(lparam);
		self->_height = HIWORD(lparam);
		break;
	case WM_SIZING: {
		RECT* rect = reinterpret_cast<RECT*>(lparam);
		self->_width = rect->right - rect->left;
		self->_height = rect->bottom - rect->top;
		break;
	}
	case WM_MOVE:
		self->_x = LOWORD(lparam);
		self->_y = HIWORD(lparam);
		break;
	default:
		return DefWindowProcA(hwnd, msg, wparam, lparam);
	}

	return 0;
}

} // namespace kengine::core::window::win

#endif // KENGINE_PLATFORM_WINDOWS