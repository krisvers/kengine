#include <kengine/macros.hpp>
#include <kengine/core/platform/platform.hpp>
#include <kengine/core/exception.hpp>

#include <stdexcept>

#include "linux/platform_linux.hpp"
#include "win/platform_win.hpp"
#include "macOS/platform_macOS.hpp"

namespace kengine::core::platform {

IPlatform& Platform::create() {
	if (_platform != nullptr) {
		throw kengine::core::Exception("Platform already created");
	}

#ifdef KENGINE_PLATFORM_WINDOWS
	IPlatform& plat = *new win::PlatformWin();
	#elif defined(KENGINE_PLATFORM_LINUX)
	IPlatform& plat = *new linux::PlatformLinux();
	#elif defined(KENGINE_PLATFORM_MACOS)
	IPlatform& plat = *new macOS::PlatformMacOS();
#endif

	_platform = &plat;
	return plat;
}

void Platform::destroy(IPlatform& platform) {
	_platform = nullptr;
	delete &platform;
}

} // namespace kengine::core::platform