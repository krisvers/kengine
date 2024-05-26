#include "platform_linux.hpp"

#ifdef KENGINE_PLATFORM_LINUX

namespace kengine::core::platform::linux {

bool PlatformLinux::init() {
	return true;
}

void PlatformLinux::shutdown() {

}

bool PlatformLinux::update() {
	return true;
}

kengine::f64 PlatformLinux::getTime() {
	return 0;
}

} // namespace kengine::core::platform::linux

#endif // KENGINE_PLATFORM_LINUX
