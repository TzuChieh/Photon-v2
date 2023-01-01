#include "Common/logging.h"
#include "Common/assertion.h"
#include "Common/Logger.h"
#include "Utility/Timestamp.h"

#include <utility>
#include <mutex>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>

namespace ph::detail::core_logging
{

namespace
{

std::mutex& CORE_LOGGER_MUTEX()
{
	static std::mutex locker;
	return locker;
}

LogGroups& CORE_LOG_GROUPS()
{
	static LogGroups groups;
	return groups;
}

Logger make_core_logger()
{
	Logger logger;

	// Wrap a lock around StdOutLogPrinter to avoid potential race between threads. This only protect 
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
			std::lock_guard<std::mutex> lock(CORE_LOGGER_MUTEX());

			stdOutLogPrinter(logLevel, logString);
		});

	auto coreLogFilename = Timestamp().toYMDHMS() + "_core_logs.txt";
	auto coreLogFilePath = std::string(PH_LOG_FILE_DIRECTRY) + coreLogFilename;

	// Replace any ':' in the path with '-' as some OS may not allow it
	std::replace(coreLogFilePath.begin(), coreLogFilePath.end(), ':', '-');

	// Possibly create non-existing directory first otherwise std::ofstream
	// will result in an error
	std::filesystem::create_directories(PH_LOG_FILE_DIRECTRY);

	// FIXME: we should explicitly open/close stream/logger in engine init/exit code. static order should
	// not be relied upon as it can cause error in dtors
	// A `static` stream as there should be only one core log file ever created
	// per engine run
	static std::ofstream stream(coreLogFilePath, std::ios_base::out);

	const auto coreLogFileAbsPath = std::filesystem::absolute(coreLogFilePath);
	if(stream.good())
	{
		std::cout << "log file <" << coreLogFileAbsPath << "> created" << '\n';
	}
	else
	{
		std::cerr << "warning: log file <" << coreLogFileAbsPath << "> creation failed" << '\n';
	}

	logger.addLogHandler(
		[](const ELogLevel logLevel, const std::string_view logString)
		{
			std::lock_guard<std::mutex> lock(CORE_LOGGER_MUTEX());

			// TODO: attempt to re-create the log file
			if(!stream.good())
			{
				return;
			}

			PH_ASSERT(stream.good());

			// Write & flush the stream immediately as we should assume the system would
			// crash at any time, so that valuable information could be kept. Note that
			// std::endl will perform both adding a newline and flushing the stream.
			stream << logString << std::endl;
		});
	
	return logger;
}

}// end namespace

std::size_t add_log_group(const std::string_view groupName, const std::string_view category)
{
	PH_ASSERT(!groupName.empty());

	std::lock_guard<std::mutex> lock(CORE_LOGGER_MUTEX());

	return CORE_LOG_GROUPS().addGroup(groupName, category);
}

// FIXME: we should explicitly open/close stream/logger in engine init/exit code. static order should
// not be relied upon as it can cause error in dtors; better, create using dynamic allocation (pointers)
// so we can easily detect uninitialized core logger
Logger& CORE_LOGGER()
{
	static Logger logger = make_core_logger();
	return logger;
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
	std::lock_guard<std::mutex> lock(detail::core_logging::CORE_LOGGER_MUTEX());

	return detail::core_logging::CORE_LOG_GROUPS();
}

}// end namespace ph
