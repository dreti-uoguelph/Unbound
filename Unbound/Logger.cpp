#include "stdafx.h"
#include "Logger.h"

namespace Unbound
{
	Logger* Logger::instance = nullptr;

	Logger* Logger::get()
	{
		if (instance == nullptr)
		{
			instance = new Logger();
		}

		return instance;
	}

	void Logger::log(const std::string& tolog)
	{
#ifdef _DEBUG
		if (tolog.substr(0, 8) == "Warning:" || tolog.substr(0, 8) == "warning:")
		{
			logWarning(tolog);
			return;
		}
		else if (tolog.substr(0, 6) == "Error:" || tolog.substr(0, 6) == "error:")
		{
			logError(tolog);
			return;
		}

		auto time = Util::Time::get()->getCurrentTime().asTimeAndDate();
		std::string message = "[";
		message += time;
		message += "]:	";
		message += tolog;
		message += '\n';

#ifdef _WIN32
		SetConsoleTextAttribute(get()->consoleHandle, 7);
#endif

		std::cout << message;

		get()->filehandle.write(message.c_str(), message.length());
#endif
	}

	/*Log a warning, which will be printed in a new color and marked clearly*/
	void Logger::logWarning(const std::string& toWrite)
	{
#ifdef _DEBUG
		auto time = Util::Time::get()->getCurrentTime().asTimeAndDate();
		std::string message = "[";
		message += time;
		message += "]:	";
		message += toWrite;
		message += '\n';

#ifdef _WIN32
		SetConsoleTextAttribute(get()->consoleHandle, 14);
#endif
		std::cout << message;

		get()->filehandle.write(message.c_str(), message.length());
#endif
	}

	/*Log an error, which will be printed in red and also trigger error detection*/
	void Logger::logError(const std::string& toWrite)
	{
#ifdef _DEBUG
		instance->numErrors++;

		auto time = Util::Time::get()->getCurrentTime().asTimeAndDate();
		std::string message = "[";
		message += time;
		message += "]:	";
		message += toWrite;
		message += '\n';

#ifdef _WIN32
		SetConsoleTextAttribute(get()->consoleHandle, 12);
#endif

		std::cout << message;

		get()->filehandle.write(message.c_str(), message.length());
#endif
	}

	/*Returns the number of errors registered in the logger*/
	const uint32_t Logger::getNumErrors()
	{
#ifdef _DEBUG
		return instance->numErrors;
#else
		return 0;
#endif
	}

	Logger::Logger() noexcept
	{
		numErrors = 0;

#ifdef _DEBUG
		//Open a new log file
		filehandle.open("log.txt", std::ios::out);

		//Get the console handle on windows
#ifdef _WIN32
		consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
#endif
	}

	/*Inserts an empty line into the output, useful for separating sections*/
	void Logger::newLine()
	{
#ifdef _DEBUG
		std::cout << std::endl;
		get()->filehandle.write("\n", 1);
#endif
	}

	Logger::~Logger()
	{
		filehandle.close();
	}
}