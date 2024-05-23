#ifndef KENGINE_CORE_LOGGING_HPP
#define KENGINE_CORE_LOGGING_HPP

#include <iostream>
#include <cstdio>

namespace kengine::core {

class ILogger {
public:
	virtual void log(std::string const& message) = 0;

	void logf(std::string const& format) {
		log(format);
	}

	template<typename... Args>
	void logf(std::string const& format, Args... args) {
		std::stringstream s;
		logfConcat(s, format, args...);
		log(s.str());
	}

private:
	template<typename T, typename... Args>
	void logfConcat(std::stringstream const& sstream, std::string const& format, T value, Args... args) {
		kengine::usize index = format.find("{}");
		if (index == std::string::npos) {
			sstream << format;
			return;
		}

		sstream << format.substr(0, index);
		sstream << value;
		logfConcat(sstream, format.substr(index + 2), args...);
	}
};

class StreamLogger : public ILogger {
public:
	StreamLogger(std::ostream& s) : stream(s) {}

	void log(std::string const& message) override {
		stream << message << std::endl;
	}

private:
	std::ostream& stream;
};

class CFileLogger : public ILogger {
public:
	CFileLogger(std::FILE* f) : file(f) {}

	void log(std::string const& message) override {
		std::fprintf(file, "%s\n", message.c_str());
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

	static void log(std::string const& message) {
		if (logger == nullptr) {
			logger = new StreamLogger(std::cout);
		}

		logger->log(message);
	}

	static void logf(std::string const& format) {
		if (logger == nullptr) {
			logger = new StreamLogger(std::cout);
		}

		logger->logf(format);
	}

	template<typename... Args>
	static void logf(std::string const& format, Args... args) {
		if (logger == nullptr) {
			logger = new StreamLogger(std::cout);
		}

		logger->logf(format, args...);
	}

private:
	inline static ILogger* logger;
};

} // namespace kengine::core

#endif