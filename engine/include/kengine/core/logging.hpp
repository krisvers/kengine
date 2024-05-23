#ifndef KENGINE_CORE_LOGGING_HPP
#define KENGINE_CORE_LOGGING_HPP

#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include <kengine/types.hpp>

namespace kengine::core {

enum class LogSeverity {
	TRACE = -3,
	VERBOSE = -2,
	DEBUG = -1,
	INFO = 0,
	WARNING = 1,
	ERROR = 2,
	FATAL = 3,
};

class ILogger {
public:
	virtual void log(LogSeverity severity, std::string const& message) = 0;

	void severityAsString(LogSeverity severity, std::string& string) {
		switch (severity) {
		case LogSeverity::TRACE:
			string = "TRACE";
			break;
		case LogSeverity::VERBOSE:
			string = "VERBOSE";
			break;
		case LogSeverity::DEBUG:
			string = "DEBUG";
			break;
		case LogSeverity::INFO:
			string = "INFO";
			break;
		case LogSeverity::WARNING:
			string = "WARNING";
			break;
		case LogSeverity::ERROR:
			string = "ERROR";
			break;
		case LogSeverity::FATAL:
			string = "FATAL";
			break;
		default:
			string = "UNKNOWN";
		}
	}

	void logf(LogSeverity severity, std::string const& format) {
		log(severity, format);
	}

	template<typename... Args>
	void logf(LogSeverity severity, std::string const& format, Args... args) {
		std::stringstream s;
		logfConcat(s, format, args...);
		log(severity, s.str());
	}

private:
	template<typename T>
	kengine::usize logfConcat(std::stringstream& sstream, std::string const& format, T value) {
		kengine::usize index = format.find("{}");
		if (index == std::string::npos) {
			sstream << format;
			return index;
		}

		sstream << format.substr(0, index);
		sstream << value;
		return index;
	}

	template<typename T, typename... Args>
	kengine::usize logfConcat(std::stringstream& sstream, std::string const& format, T value, Args... args) {
		kengine::usize index = logfConcat(sstream, format, value);
		return logfConcat(sstream, format.substr(index + 2), args...);
	}
};

class StreamLogger : public ILogger {
public:
	StreamLogger(std::ostream& s) : stream(s) {}

	void log(LogSeverity severity, std::string const& message) override {
		std::string sev;
		severityAsString(severity, sev);
		stream << "[" << sev << "] " << message << std::endl;
	}

private:
	std::ostream& stream;
};

class CFileLogger : public ILogger {
public:
	CFileLogger(std::FILE* f) : file(f) {}

	void log(LogSeverity severity, std::string const& message) override {
		std::string sev;
		severityAsString(severity, sev);
		std::fprintf(file, "[%s] %s\n", sev.c_str(), message.c_str());
	}

private:
	FILE* file;
};

class Logger {
public:
	Logger() {
		if (logger != nullptr) {
			throw std::runtime_error("Logger already initialized");
		}
		
		logger = new StreamLogger(std::cout);
	}

	Logger(ILogger* l) {
		if (logger != nullptr) {
			throw std::runtime_error("Logger already initialized");
		}

		logger = l;
	}

	Logger(FILE* file) {
		if (logger != nullptr) {
			throw std::runtime_error("Logger already initialized");
		}

		logger = new CFileLogger(file);
	}

	Logger(std::ostream& s) {
		if (logger != nullptr) {
			throw std::runtime_error("Logger already initialized");
		}

		logger = new StreamLogger(s);
	}

	~Logger() { delete logger; }

	static void log(LogSeverity severity, std::string const& message) {
		if (logger == nullptr) {
			logger = new StreamLogger(std::cout);
		}

		logger->log(severity, message);
	}

	static void logf(LogSeverity severity, std::string const& format) {
		if (logger == nullptr) {
			logger = new StreamLogger(std::cout);
		}

		logger->logf(severity, format);
	}

	template<typename... Args>
	static void logf(LogSeverity severity, std::string const& format, Args... args) {
		if (logger == nullptr) {
			logger = new StreamLogger(std::cout);
		}

		logger->logf(severity, format, args...);
	}

private:
	inline static ILogger* logger;
};

} // namespace kengine::core

#endif
