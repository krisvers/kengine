#ifndef KENGINE_CORE_EXCEPTION_HPP
#define KENGINE_CORE_EXCEPTION_HPP

#include <exception>
#include <kengine/core/logging.hpp>

namespace kengine::core {

class Exception : public std::exception {
public:
	Exception() {
		Logger::get().logf(LogSeverity::Error, "KEngine Exception (no message)");
	}

	Exception(char const* message) : _message(message) {
		Logger::get().logf(LogSeverity::Error, "KEngine Exception: {}", message);
	}

	Exception(std::string const& message) : _message(message) {
		Logger::get().logf(LogSeverity::Error, "KEngine Exception: {}", message);
	}

	template<typename... Args>
	Exception(std::string const& format, Args... args) {
		std::string fmt = "KEngine Exception: ";
		Logger::get().logf(LogSeverity::Error, fmt + format, args...);
	}

	char const* what() const noexcept override { return _message.c_str(); }

private:
	std::string _message;
};

} // namespace kengine::core

#endif