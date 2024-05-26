#ifndef KENGINE_CORE_PLATFORM_WIN_PLATFORM_WIN_HPP
#define KENGINE_CORE_PLATFORM_WIN_PLATFORM_WIN_HPP

#include <kengine/macros.hpp>

#ifdef KENGINE_PLATFORM_WINDOWS

#include <kengine/core/platform/platform.hpp>

namespace kengine::core::platform::win {

class PlatformWin : public IPlatform {
public:
	bool init() override;
	void shutdown() override;
	bool update() override;

	kengine::f64 getTime() override;
};

} // namespace kengine::core::platform::win

#endif // KENGINE_PLATFORM_WINDOWS

#endif