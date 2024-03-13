#ifndef KRISVERS_KENGINE_LOGGER_HPP
#define KRISVERS_KENGINE_LOGGER_HPP

#include <kengine/types.hpp>

namespace kengine {

enum class LogType {
	VERBOSE = -2,
	DEBUG = -1,
	INFO = 0,
	WARNING = 1,
	ERROR = 2,
};

class logger {
public:
	static void print(LogType type, const char* string);
	static void printf(LogType type, const char* format, ...);
};

} // namespace kengine

#endif
