#pragma once

#include "Common/LogSender.h"
#include "Common/ELogLevel.h"

#include <string>

namespace ph
{

class Logger final
{
public:
	static const Logger& DEFAULT();

public:
	explicit Logger(const LogSender& logSender);

	void log(const std::string& message) const;
	void log(const ELogLevel& logLevel, const std::string& message) const;
	void setLogSender(const LogSender& logSender);

private:
	LogSender m_logSender;

	static void log(const LogSender& logSender, const ELogLevel& logLevel, const std::string& message);
};

}// end namespace ph
