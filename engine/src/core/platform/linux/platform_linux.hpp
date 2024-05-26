#ifndef KENGINE_CORE_PLATFORM_LINUX_PLATFORM_LINUX_HPP
#define KENGINE_CORE_PLATFORM_LINUX_PLATFORM_LINUX_HPP

#include <kengine/macros.hpp>

#ifdef KENGINE_PLATFORM_LINUX

#include <kengine/core/platform/platform.hpp>

#ifdef linux
#undef linux
#endif // linux

namespace kengine::core::platform::linux {

class PlatformLinux : public IPlatform {
public:
	virtual bool init() override;
	virtual void shutdown() override;
	virtual bool update() override;

	virtual kengine::f64 getTime() override;
};

} // namespace kengine::core::platform::linux

#endif // KENGINE_PLATFORM_LINUX

#endif
