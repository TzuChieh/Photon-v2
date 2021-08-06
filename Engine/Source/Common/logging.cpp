#include "Common/logging.h"
#include "Common/assertion.h"
#include "Common/Logger.h"

#include <mutex>
#include <utility>

namespace ph
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

}

std::size_t add_core_log_group(std::string groupName, std::string category)
{
	PH_ASSERT(!groupName.empty());

	std::lock_guard<std::mutex> lock(CORE_LOGGER_MUTEX());

	return CORE_LOG_GROUPS().addGroup(std::move(groupName), std::move(category));
}

LogGroups get_core_log_groups()
{
	std::lock_guard<std::mutex> lock(CORE_LOGGER_MUTEX());

	return CORE_LOG_GROUPS();
}

Logger& CORE_LOGGER()
{
	static Logger logger(LogSender(""));
	return logger;
}

}// end namespace ph
