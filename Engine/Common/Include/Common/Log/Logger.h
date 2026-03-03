#pragma once

#include "Common/Log/logger_fwd.h"

#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include <memory>

namespace ph
{

/*! @brief General logger that holds a set of log handlers.
A logger provides several @ref log methods for recording messages. The recorded messages will be
passed to all added log handlers.
*/
class Logger final
{
public:
	Logger();

	/*! @brief Log a message with @ref ELogLevel::Note level.
	*/
	void log(std::string_view message) const;

	/*! @brief Log a message with a specific log level.
	*/
	void log(ELogLevel logLevel, std::string_view message) const;

	/*! @brief Log a message with a specific group name and log level.
	*/
	void log(
		std::string_view name,
		ELogLevel        logLevel, 
		std::string_view message) const;

	/*! @brief Add a log handler that can deal with log messages.
	Log handler must be copyable.
	*/
	void addLogHandler(LogHandler logHandler);

public:
	/*! @brief Get a log handler that prints to standard output.
	*/
	static LogHandler makeStdOutLogPrinter();

	/*! @brief Get a log handler that prints to standard output with colors.
	*/
	static LogHandler makeColoredStdOutLogPrinter();

private:
	std::vector<LogHandler> m_logHandlers;

	static std::string makeLogString(
		std::string_view name,
		ELogLevel        logLevel,
		std::string_view message);

	static bool shouldStdOutPrintWithoutBuffering(ELogLevel logLevel);
};

}// end namespace ph
