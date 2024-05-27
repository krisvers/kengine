#ifndef KENGINE_CORE_PLATFORM_PLATFORM_HPP
#define KENGINE_CORE_PLATFORM_PLATFORM_HPP

#include <kengine/types.hpp>
#include <kengine/singleton.hpp>

namespace kengine::core::platform {

class IPlatform {
public:
	virtual bool init() = 0;
	virtual void shutdown() = 0;
	virtual bool update() = 0;

	virtual kengine::f64 getTime() = 0;
	virtual ~IPlatform() = default;

protected:
	IPlatform() = default;

	kengine::f64 _time = 0;
};

class Platform : public Singleton<Platform> {
public:
	IPlatform& create();
	void destroy(IPlatform& platform);

	IPlatform& getPlatform() { return *_platform; }

private:
	IPlatform* _platform;
};

} // namespace kengine::core::platform

#endif
