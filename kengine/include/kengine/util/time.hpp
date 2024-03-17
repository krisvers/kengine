#ifndef KRISVERS_KENGINE_UTIL_TIME_HPP
#define KRISVERS_KENGINE_UTIL_TIME_HPP

#include <kengine/types.hpp>

#include <chrono>

namespace kengine::util {

class time {
public:
	static void init();
	static void update();
	static f64 timeElapsed();
	static f64 totalTime();

private:
	static std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	static std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;
};

class Timer {
public:
	f64 startTime;
	f64 currentTime;
	f64 deltaTime;
	f64 endTime;

	Timer() { time::update(); start(); }
	~Timer() {}

	f64 start() { return startTime = time::timeElapsed(); }
	f64 check() { currentTime = time::timeElapsed(); deltaTime = currentTime - startTime; return currentTime; }
	f64 end() { return endTime = check(); }
};

} // namespace kengine::util

#endif