#ifndef KRISVERS_KENGINE_LOGGER_HPP
#define KRISVERS_KENGINE_LOGGER_HPP

#include <kengine/types.hpp>

#include <iostream>

namespace kengine {

enum class LogType {
	Verbose = -2,
	Debug = -1,
	Info = 0,
	Warning = 1,
	Error = 2,
};

class logger {
public:
	static void print(LogType type, const char* string);
	static void printf(LogType type, const char* format, ...);
};

} // namespace kengine

#endif
