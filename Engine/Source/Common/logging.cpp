#include "Common/logging.h"
#include "Common/assertion.h"
#include "Common/Logger.h"

#include <utility>
#include <mutex>

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

	// This is basically the same as Logger::addStdHanlder(), except that it uses
	// a mutex to avoid potential race between threads
	logger.addLogHandler(
		[](const ELogLevel logLevel, const std::string_view logString)
		{
			std::lock_guard<std::mutex> lock(CORE_LOGGER_MUTEX());

			if(logLevel == ELogLevel::NOTE)
			{
				std::cout << logString << std::endl;
			}
			else
			{
				std::cerr << logString << std::endl;
			}
		});

	// TODO: add file handler
	
	return logger;
}

}// end namespace

std::size_t add_log_group(const std::string_view groupName, const std::string_view category)
{
	PH_ASSERT(!groupName.empty());

	std::lock_guard<std::mutex> lock(CORE_LOGGER_MUTEX());

	return CORE_LOG_GROUPS().addGroup(groupName, category);
}

Logger& CORE_LOGGER()
{
	static Logger logger = make_core_logger();
	return logger;
}

void log_to_logger(Logger& logger, const std::string_view groupName, const ELogLevel logLevel, const std::string_view logMessage)
{
	std::lock_guard<std::mutex> lock(CORE_LOGGER_MUTEX());

	logger.log(groupName, logLevel, logMessage);
}

}// end namespace ph::detail::core_logging

namespace ph
{

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
