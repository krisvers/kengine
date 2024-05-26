#ifndef KENGINE_CORE_LOGGING_HPP
#define KENGINE_CORE_LOGGING_HPP

#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>

#include <kengine/types.hpp>
#include <kengine/singleton.hpp>

namespace kengine::core {

enum class LogSeverity {
	Trace = -3,
	Verbose = -2,
	Debug = -1,
	Info = 0,
	Warning = 1,
	Error = 2,
	Fatal = 3,
};

class ILogger {
public:
	virtual void log(LogSeverity severity, std::string const& message) = 0;

	void severityAsString(LogSeverity severity, std::string& string) {
		switch (severity) {
		case LogSeverity::Trace:
			string = "TRACE";
			break;
		case LogSeverity::Verbose:
			string = "VERBOSE";
			break;
		case LogSeverity::Debug:
			string = "DEBUG";
			break;
		case LogSeverity::Info:
			string = "INFO";
			break;
		case LogSeverity::Warning:
			string = "WARNING";
			break;
		case LogSeverity::Error:
			string = "ERROR";
			break;
		case LogSeverity::Fatal:
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
	std::stringstream& logfValue(std::stringstream& sstream, T const& value) {
		sstream << value;
		return sstream;
	}

	template<>
	std::stringstream& logfValue<bool>(std::stringstream& sstream, bool const& value) {
		sstream << (value ? "true" : "false");
		return sstream;
	}

	template<typename T>
	kengine::usize logfConcat(std::stringstream& sstream, std::string const& format, T const& value) {
		kengine::usize index = format.find("{}");
		if (index == std::string::npos) {
			sstream << format;
			return 0;
		}

		sstream << format.substr(0, index);
		logfValue(sstream, value) << format.substr(index + 2);
		return std::string::npos;
	}

	template<typename T, typename... Args>
	kengine::usize logfConcat(std::stringstream& sstream, std::string const& format, T const& value, Args const&... args) {
		kengine::usize index = format.find("{}");
		if (index == std::string::npos) {
			sstream << format;
			return 0;
		}

		sstream << format.substr(0, index);
		logfValue(sstream, value);

		if (index + 2 < format.size()) {
			return logfConcat(sstream, format.substr(index + 2), args...);
		}

		return index;
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

class Logger : public kengine::Singleton<Logger> {
public:
	void init() {
		if (logger != nullptr) {
			throw std::runtime_error("Logger already initialized");
		}
		
		logger = new StreamLogger(std::cout);
	}

	void init(ILogger* l) {
		if (logger != nullptr) {
			throw std::runtime_error("Logger already initialized");
		}

		logger = l;
	}

	void init(FILE* file) {
		if (logger != nullptr) {
			throw std::runtime_error("Logger already initialized");
		}

		logger = new CFileLogger(file);
	}

	void init(std::ostream& s) {
		if (logger != nullptr) {
			throw std::runtime_error("Logger already initialized");
		}

		logger = new StreamLogger(s);
	}

	void deinit() { delete logger; }

	void log(LogSeverity severity, std::string const& message) {
		if (logger == nullptr) {
			logger = new StreamLogger(std::cout);
		}

		logger->log(severity, message);
	}

	void logf(LogSeverity severity, std::string const& format) {
		if (logger == nullptr) {
			logger = new StreamLogger(std::cout);
		}

		logger->logf(severity, format);
	}

	template<typename... Args>
	void logf(LogSeverity severity, std::string const& format, Args... args) {
		if (logger == nullptr) {
			logger = new StreamLogger(std::cout);
		}

		logger->logf(severity, format, args...);
	}

	ILogger* getLogger() { return logger; }

private:
	ILogger* logger;
};

} // namespace kengine::core

#endif
