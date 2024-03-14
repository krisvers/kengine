#include <kengine/logger.hpp>
#include <cstdio>
#include <cstdarg>

namespace kengine {

void logger::print(LogType type, const char* string) {
	const char* types[5] = { "[VERBOSE]", "[DEBUG]", "[INFO]", "[WARNING]", "[ERROR]" };
	const char* typestring = ((type == LogType::VERBOSE) ? types[0] : (type == LogType::DEBUG) ? types[1] : (type == LogType::INFO) ? types[2] : (type == LogType::WARNING) ? types[3] : types[4]);
	std::printf("%s %s", typestring, string);
}

void logger::printf(LogType type, const char* format, ...) {
	const char* types[5] = { "[VERBOSE]", "[DEBUG]", "[INFO]", "[WARNING]", "[ERROR]" };
	const char* typestring = ((type == LogType::VERBOSE) ? types[0] : (type == LogType::DEBUG) ? types[1] : (type == LogType::INFO) ? types[2] : (type == LogType::WARNING) ? types[3] : types[4]);
	std::printf("%s ", typestring);

	va_list list;
	va_start(list, format);
	std::vprintf(format, list);
	va_end(list);
}

} // namespace kengine
