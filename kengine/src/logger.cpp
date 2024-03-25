#include <kengine/logger.hpp>
#include <cstdio>
#include <cstdarg>

namespace kengine {

void logger::print(LogType type, const char* string) {
	const char* types[5] = { "[VERBOSE]", "[DEBUG]", "[INFO]", "[WARNING]", "[ERROR]" };
	const char* typestring = ((type == LogType::Verbose) ? types[0] : (type == LogType::Debug) ? types[1] : (type == LogType::Info) ? types[2] : (type == LogType::Warning) ? types[3] : types[4]);
	std::printf("%s %s", typestring, string);
}

void logger::printf(LogType type, const char* format, ...) {
	const char* types[5] = { "[VERBOSE]", "[DEBUG]", "[INFO]", "[WARNING]", "[ERROR]" };
	const char* typestring = ((type == LogType::Verbose) ? types[0] : (type == LogType::Debug) ? types[1] : (type == LogType::Info) ? types[2] : (type == LogType::Warning) ? types[3] : types[4]);
	std::printf("%s ", typestring);

	va_list list;
	va_start(list, format);
	std::vprintf(format, list);
	va_end(list);
}

} // namespace kengine
