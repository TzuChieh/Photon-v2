#include "Common/logging.h"
#include "Common/Log/Logger.h"
#include "Utility/Timestamp.h"

#include <Common/assertion.h>

#include <utility>
#include <mutex>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <optional>

namespace ph::detail::core_logging
{

namespace
{

std::mutex g_coreLogMutex;
LogGroups g_coreLogGroups;

/*!
We explicitly open/close stream/logger in engine init/exit code. Static order should not be relied 
upon as it can cause error in dtors of statics. Errors regarding uninitialized logging utilities would 
likely be logging in statics (not within engine init & exit).
*/
///@{
std::ofstream g_coreLogStream;
std::optional<Logger> g_coreLogger;
///@}

Logger make_core_logger()
{
	Logger logger;

	// Wrap a lock around `stdOutLogPrinter` to avoid potential race between threads. This only protect 
	// against the core logger itself--logging from multiple threads will not cause interleaved characters
	// in the output, but others using std::cout/cerr concurrently might still interleave their characters
	// into core logger's output. Still, it is safe to access standard output stream from multiple threads
	// as long as `sync_with_stdio(true)` is set. 
	// 
	// See: https://en.cppreference.com/w/cpp/io/cout
	//      https://stackoverflow.com/questions/28660602/synchronizing-output-to-stdcout
	//
	logger.addLogHandler(
		[stdOutLogPrinter = Logger::makeColoredStdOutLogPrinter()]
		(const ELogLevel logLevel, const std::string_view logString)
		{
			std::lock_guard<std::mutex> lock(g_coreLogMutex);

			stdOutLogPrinter(logLevel, logString);
		});

	logger.addLogHandler(
		[](const ELogLevel logLevel, const std::string_view logString)
		{
			std::lock_guard<std::mutex> lock(g_coreLogMutex);

			// TODO: attempt to re-create the log file
			if(!g_coreLogStream.good())
			{
				return;
			}

			PH_ASSERT(g_coreLogStream.good());

			// Write & flush the stream immediately as we should assume the system would
			// crash at any time, so that valuable information could be kept. Note that
			// std::endl will perform both adding a newline and flushing the stream.
			g_coreLogStream << logString << std::endl;
		});
	
	return logger;
}

}// end namespace

void init()
{
	// In case some threads were created before `init()`
	std::lock_guard<std::mutex> lock(g_coreLogMutex);

	PH_ASSERT_MSG(!g_coreLogger.has_value(),
		"Logger is already initialized.");

	auto coreLogFilename = Timestamp().toYMDHMS() + "_core_logs.txt";
	auto coreLogFilePath = std::string(PH_LOG_FILE_DIRECTRY) + coreLogFilename;

	// Replace any ':' in the path with '-' as some OS may not allow it
	std::replace(coreLogFilePath.begin(), coreLogFilePath.end(), ':', '-');

	// Possibly create non-existing directory first otherwise std::ofstream
	// will result in an error
	std::filesystem::create_directories(PH_LOG_FILE_DIRECTRY);

	// A global stream as there should be only one core log file ever created per engine run
	PH_ASSERT(!g_coreLogStream.is_open());
	g_coreLogStream = std::ofstream(coreLogFilePath, std::ios_base::out);

	const auto coreLogFileAbsPath = std::filesystem::absolute(coreLogFilePath);
	if(g_coreLogStream.good())
	{
		std::cout << "log file <" << coreLogFileAbsPath << "> created" << '\n';
	}
	else
	{
		std::cerr << "warning: log file <" << coreLogFileAbsPath << "> creation failed" << '\n';
	}

	g_coreLogger = make_core_logger();
}

void exit()
{
	// Make sure all logs are visible before quitting
	std::lock_guard<std::mutex> lock(g_coreLogMutex);

	PH_ASSERT(g_coreLogger.has_value());
	g_coreLogger = std::nullopt;

	PH_ASSERT(g_coreLogStream.is_open());
	g_coreLogStream.close();
}

Logger& get_core_logger()
{
	PH_ASSERT_MSG(g_coreLogger.has_value(),
		"Using logger without initializing.");

	return *g_coreLogger;
}

std::size_t add_log_group(const std::string_view groupName, const std::string_view category)
{
	PH_ASSERT(!groupName.empty());

	std::lock_guard<std::mutex> lock(g_coreLogMutex);

	PH_ASSERT(g_coreLogger.has_value());
	return g_coreLogGroups.addGroup(groupName, category);
}

void log_to_logger(Logger& logger, const std::string_view groupName, const ELogLevel logLevel, const std::string_view logMessage)
{
	logger.log(groupName, logLevel, logMessage);
}

}// end namespace ph::detail::core_logging

namespace ph
{

PH_DEFINE_LOG_GROUP(PhotonRenderer, Core);

std::size_t LogGroups::addGroup(const std::string_view groupName, const std::string_view category)
{
	m_groups.push_back({std::string(groupName), std::string(category)});
	return m_groups.size() - 1;
}

std::size_t LogGroups::numGroups() const
{
	return m_groups.size();
}

const LogGroup& LogGroups::getGroup(const std::size_t index) const
{
	PH_ASSERT_LT(index, m_groups.size());

	return m_groups[index];
}

LogGroups get_core_log_groups()
{
	std::lock_guard<std::mutex> lock(detail::core_logging::g_coreLogMutex);

	return detail::core_logging::g_coreLogGroups;
}

}// end namespace ph
