#include "platform_win.hpp"

#include <kengine/macros.hpp>

#ifdef KENGINE_PLATFORM_WINDOWS

#include <Windows.h>

namespace kengine::core::platform::win {

bool PlatformWin::init() {
	return true;
}

void PlatformWin::shutdown() {

}

bool PlatformWin::update() {
	MSG msg;
	while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return true;
}

kengine::f64 PlatformWin::getTime() {
	return _time;
}

} // namespace kengine::core::platform::win

#endif // KENGINE_PLATFORM_WINDOWS