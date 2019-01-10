#pragma once
#include"stdafx.h"
#include<fstream>
#include"TimeManager.h"
#ifdef _WIN32
#undef APIENTRY
#include<Windows.h>
#endif

namespace Unbound
{
	/*This class is responsible for displaying and recording debug information.
	It will automatically print to the console and record info in a log file for future use*/
	class Logger
	{
	public:
		std::ofstream filehandle; //The handle to the current log file being written

//platform specific code for changing text color
#ifdef _WIN32
		HANDLE consoleHandle;
#endif

		/*A string can be passed into the logger to be written automatically*/
		static void log(const std::string& toWrite);
		/*Log a warning, which will be printed in a new color and marked clearly*/
		static void logWarning(const std::string& toWrite);
		/*Log an error, which will be printed in red and also trigger error detection*/
		static void logError(const std::string& toWrite);
		/*Inserts an empty line into the output, useful for separating sections*/
		static void newLine();

		/*Returns the number of errors registered in the logger*/
		static const uint32_t getNumErrors();

		Logger() noexcept;
		~Logger();
	private:
		static Logger* instance;
		static Logger* get(); //Get the current logger instance
		uint32_t numErrors; //Counts the number of errors encountered while running the software
	};
}