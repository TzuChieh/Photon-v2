#pragma once

#include "Common/ELogLevel.h"

#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include <memory>

namespace ph
{

class Logger final
{
public:
	using LogHandler = std::function<void(ELogLevel logLevel, std::string_view logString)>;

	Logger();

	void log(std::string_view message) const;

	void log(ELogLevel logLevel, std::string_view message) const;

	void log(
		std::string_view name,
		ELogLevel        logLevel, 
		std::string_view message) const;

	/*! @brief Add a log handler that can deal with log messages.
	
	Log handler must be copyable.
	*/
	void addLogHandler(LogHandler logHandler);

public:
	static LogHandler makeStdOutLogPrinter();
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
