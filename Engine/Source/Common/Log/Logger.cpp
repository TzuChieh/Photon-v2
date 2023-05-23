#include "Common/Log/Logger.h"
#include "Common/Log/ELogLevel.h"
#include "Utility/Timestamp.h"
#include "Common/config.h"
#include "Common/os.h"

#include <iostream>
#include <string>
#include <utility>

#ifdef PH_OPERATING_SYSTEM_IS_WINDOWS
	
#include <Windows.h>

#endif

namespace ph
{

Logger::Logger() :
	m_logHandlers()
{}

void Logger::log(const std::string_view message) const
{
	log(ELogLevel::Note, message);
}

void Logger::log(const ELogLevel logLevel, const std::string_view message) const
{
	log("", logLevel, message);
}

void Logger::log(
	const std::string_view name,
	const ELogLevel        logLevel,
	const std::string_view message) const
{
	const std::string logString = makeLogString(name, logLevel, message);

	for(const LogHandler& logHandler : m_logHandlers)
	{
		logHandler(logLevel, logString);
	}
}

void Logger::addLogHandler(LogHandler logHandler)
{
	m_logHandlers.push_back(std::move(logHandler));
}

std::string Logger::makeLogString(
	const std::string_view name,
	const ELogLevel        logLevel,
	const std::string_view message)
{
	const Timestamp timestamp;
	
	std::string logMessage;
	logMessage.reserve(256);

	logMessage += "[";
	logMessage += timestamp.toYMDHMSMilliseconds();
	logMessage += "] ";

	if(!name.empty())
	{
		logMessage += "[";
		logMessage += name;
		logMessage += "] ";
	}

	if(logLevel != ELogLevel::Note)
	{
		switch(logLevel)
		{
		case ELogLevel::Debug:   logMessage += "[DEBUG] ";   break;
		case ELogLevel::Warning: logMessage += "[WARNING] "; break;
		case ELogLevel::Error:   logMessage += "[ERROR] ";   break;

		default: logMessage += "[UNKNOWN] "; break;
		}
	}

	logMessage += message;

	return std::move(logMessage);
}

bool Logger::shouldStdOutPrintWithoutBuffering(const ELogLevel logLevel)
{
	return logLevel == ELogLevel::Error || logLevel == ELogLevel::Debug;
}

auto Logger::makeStdOutLogPrinter() -> LogHandler
{
	return [](const ELogLevel logLevel, const std::string_view logString)
	{
		if(shouldStdOutPrintWithoutBuffering(logLevel))
		{
			// Note that printing to cerr will flush cout first, automatically
			std::cerr << logString << '\n';
		}
		else
		{
			std::cout << logString << '\n';
		}
	};
}

auto Logger::makeColoredStdOutLogPrinter() -> LogHandler
{
#ifdef PH_OPERATING_SYSTEM_IS_WINDOWS
	return [stdOutLogPrinter = makeStdOutLogPrinter()](const ELogLevel logLevel, const std::string_view logString)
	{
		/*
			For a color 0xXY, X = background color, Y = foreground color,
			available colors are:

			0 = Black
			8 = Gray
			1 = Blue
			9 = Light Blue
			2 = Green
			A = Light Green
			3 = Aqua
			B = Light Aqua
			4 = Red
			C = Light Red
			5 = Purple
			D = Light Purple
			6 = Yellow
			E = Light Yellow
			7 = White
			F = Bright White
		*/

		constexpr WORD WARNING_COLOR = 0x0E;// light yellow
		constexpr WORD ERROR_COLOR   = 0x0C;// light red
		constexpr WORD DEBUG_COLOR   = 0x09;// light blue

		HANDLE stdOutHandle = shouldStdOutPrintWithoutBuffering(logLevel) ? 
			GetStdHandle(STD_ERROR_HANDLE) : GetStdHandle(STD_OUTPUT_HANDLE);

		// Record console settings before modifying them so we can restore them later
		CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufInfo;
		GetConsoleScreenBufferInfo(stdOutHandle, &consoleScreenBufInfo);

		// We need to flush first so any buffered content will not get wrong color
		std::cout << std::flush;

		// Set color & print in console
		switch(logLevel)
		{
		case ELogLevel::Warning:
			SetConsoleTextAttribute(stdOutHandle, WARNING_COLOR);
			break;

		case ELogLevel::Error:
			SetConsoleTextAttribute(stdOutHandle, ERROR_COLOR);
			break;

		case ELogLevel::Debug:
			SetConsoleTextAttribute(stdOutHandle, DEBUG_COLOR);
			break;
		}

		// Print by wrapped standard output stream printer
		stdOutLogPrinter(logLevel, logString);

		// We need to flush buffered output so they can use current color
		if(!shouldStdOutPrintWithoutBuffering(logLevel))
		{
			std::cout << std::flush;
		}

		// Restore console settings
		SetConsoleTextAttribute(stdOutHandle, consoleScreenBufInfo.wAttributes);
	};
#else
	// TODO: impl ANSI-based coloring; currently it is un-colored
	// https://stackoverflow.com/questions/4053837/colorizing-text-in-the-console-with-c
	return makeStdOutLogPrinter();
#endif
}

}// end namespace ph
