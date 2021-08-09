#include "Common/Logger.h"
#include "Utility/Timestamp.h"
#include "Common/config.h"

#include <iostream>
#include <string>
#include <utility>

// FIXME: currently windows headers are non-standard, could not color console 
// output on windows
//#if _WIN32 || _WIN64
//	#define OPERATING_SYSTEM_WINDOWS
//	#include <windows.h>
//#endif

namespace ph
{

Logger::Logger() :
	m_logHandlers()
{}

void Logger::log(const std::string_view message) const
{
	log(ELogLevel::NOTE, message);
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

void Logger::addStdHandler()
{
	addLogHandler(
		[](const ELogLevel logLevel, const std::string_view logString)
		{
			if(logLevel == ELogLevel::NOTE)
			{
				std::cout << logString << std::endl;
			}
			else
			{
				std::cerr << logString << std::endl;
			}
		});
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
	logMessage += timestamp.toString();
	logMessage += "] ";

	if(!name.empty())
	{
		logMessage += "[";
		logMessage += name;
		logMessage += "] ";
	}

	if(logLevel != ELogLevel::NOTE)
	{
		switch(logLevel)
		{
		case ELogLevel::DEBUG:   logMessage += "[DEBUG] ";   break;
		case ELogLevel::WARNING: logMessage += "[WARNING] "; break;
		case ELogLevel::ERROR:   logMessage += "[ERROR] ";   break;

		default: logMessage += "[UNKNOWN] "; break;
		}
	}

	logMessage += message;

	return std::move(logMessage);
}

//std::string Logger::makeLogString(
//	const std::string& name,
//	const ELogLevel& logLevel,
//	const std::string& message)
//{
//	const Timestamp timestamp;
//
//#ifdef OPERATING_SYSTEM_WINDOWS
//	/*
//		For a color 0xAB, A = background color, B = foreground color,
//		available colors are:
//
//		0 = Black
//		8 = Gray
//		1 = Blue
//		9 = Light Blue
//		2 = Green
//		A = Light Green
//		3 = Aqua
//		B = Light Aqua
//		4 = Red
//		C = Light Red
//		5 = Purple
//		D = Light Purple
//		6 = Yellow
//		E = Light Yellow
//		7 = White
//		F = Bright White
//	*/
//
//	const WORD messageColor = 0x08;// gray
//	const WORD warningColor = 0x0E;// light yellow
//	const WORD errorColor   = 0x0C;// light red
//	const WORD debugColor   = 0x09;// light blue
//
//	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
//
//	// record console settings before modifying them so we can restore them later
//	CONSOLE_SCREEN_BUFFER_INFO csbi;
//	GetConsoleScreenBufferInfo(hstdout, &csbi);
//
//	// set color & print in console
//	switch(logLevel)
//	{
//	case ELogLevel::NOTE_MESSAGE:
//		SetConsoleTextAttribute(hstdout, messageColor);
//		std::cout << "[" << logSender.getSenderName() << "] >> " << message << std::endl;
//		break;
//
//	case ELogLevel::NOTE_WARNING:
//		SetConsoleTextAttribute(hstdout, warningColor);
//		std::cerr << "[" << logSender.getSenderName() << "] warning >> " << message << std::endl;
//		break;
//
//	case ELogLevel::RECOVERABLE_ERROR:
//	case ELogLevel::SEVERE_ERROR:
//	case ELogLevel::FATAL_ERROR:
//		SetConsoleTextAttribute(hstdout, errorColor);
//		std::cerr << "[" << logSender.getSenderName() << "] error >> " << message << std::endl;
//		break;
//
//	case ELogLevel::DEBUG_MIN:
//	case ELogLevel::DEBUG_MED:
//	case ELogLevel::DEBUG_MAX:
//		SetConsoleTextAttribute(hstdout, debugColor);
//		std::cerr << "[" << logSender.getSenderName() << "] debug >> " << message << std::endl;
//		break;
//
//	default:
//		// set console text color
//		SetConsoleTextAttribute(hstdout, warningColor);
//		std::cerr << "Logger Warning: use of unsupported log level" << std::endl;
//		std::cerr << "message: " << message << std::endl;
//		std::cerr << "from: " << logSender.getSenderName() << std::endl;
//		break;
//	}
//
//	// restore console settings
//	SetConsoleTextAttribute(hstdout, csbi.wAttributes);
//
//#else
//
//	switch(logLevel)
//	{
//	case ELogLevel::NOTE_MIN:
//	case ELogLevel::NOTE_MED:
//	case ELogLevel::NOTE_MAX:
//#ifndef PH_UNBUFFERED_LOG
//		std::cout << "[" + timestamp.toString() + "] " 
//		          << "[" << logSender.getSenderName() << "] " << message << std::endl;
//#else
//		std::cerr << "[" + timestamp.toString() + "] "
//		          << "[" << logSender.getSenderName() << "] " << message << std::endl;
//#endif
//		break;
//
//	case ELogLevel::WARNING_MIN:
//	case ELogLevel::WARNING_MED:
//	case ELogLevel::WARNING_MAX:
//		std::cerr << "[" + timestamp.toString() + "] " 
//		          << "[" << logSender.getSenderName() << "] [WARNING] " << message << std::endl;
//		break;
//
//	case ELogLevel::DEBUG_MIN:
//	case ELogLevel::DEBUG_MED:
//	case ELogLevel::DEBUG_MAX:
//		std::cerr << "[" + timestamp.toString() + "] " 
//		          << "[" << logSender.getSenderName() << "] [DEBUG] " << message << std::endl;
//		break;
//
//	case ELogLevel::RECOVERABLE_ERROR:
//	case ELogLevel::SEVERE_ERROR:
//	case ELogLevel::FATAL_ERROR:
//		std::cerr << "[" + timestamp.toString() + "] " 
//		          << "[" << logSender.getSenderName() << "] [ERROR] " << message << std::endl;
//		break;
//
//	default:
//		std::cerr << "Logger Warning: use of unsupported log level" << std::endl;
//		std::cerr << "message: " << message << std::endl;
//		std::cerr << "from: " << logSender.getSenderName() << std::endl;
//		break;
//	}
//
//#endif
//
//	if(logLevel == ELogLevel::FATAL_ERROR)
//	{
//		exit(EXIT_FAILURE);
//	}
//
//}// end Logger::log()

}// end namespace ph