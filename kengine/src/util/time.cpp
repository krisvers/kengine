#include <kengine/util/time.hpp>

#include <chrono>

namespace kengine::util {

std::chrono::time_point<std::chrono::high_resolution_clock> time::startTime;
std::chrono::time_point<std::chrono::high_resolution_clock> time::currentTime;

void time::init() {
	startTime = std::chrono::high_resolution_clock::now();
	currentTime = startTime;
}

void time::update() {
	currentTime = std::chrono::high_resolution_clock::now();
}

f64 time::timeElapsed() {
	update();
	return std::chrono::duration<f64>(currentTime - startTime).count();
}

f64 time::totalTime() {
	update();
	return std::chrono::duration<f64>(currentTime.time_since_epoch()).count();
}

} // namespace kengine::util