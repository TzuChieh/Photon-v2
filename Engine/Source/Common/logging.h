#pragma once

#include <string>
#include <vector>
#include <cstddef>
#include <format>

namespace ph { class Logger; }

namespace ph
{

struct LogGroup final
{
public:
	std::string groupName;
	std::string category;
};

class LogGroups final
{
public:
	inline LogGroups() = default;
	inline LogGroups(const LogGroups& other) = default;
	inline LogGroups& operator = (const LogGroups& rhs) = default;

	std::size_t addGroup(std::string groupName, std::string category = "");
	const LogGroup& getGroup(std::size_t index) const;

private:
	std::vector<LogGroup> m_groups;
};

std::size_t add_core_log_group(std::string groupName, std::string category = "");
LogGroups get_core_log_groups();

}// end namespace ph

namespace ph::detail
{

// TODO; thread safety
Logger& CORE_LOGGER();

}// end namespace ph::detail

#define PH_DECLARE_LOG_GROUP(groupName)\
	inline ::ph::Logger& internal_impl_logger_access_##groupName##()

#define PH_DEFINE_LOG_GROUP(groupName, category)\
	::ph::Logger& internal_impl_logger_access_##groupName##()\
	{\
		static const std::size_t logGroupIndex = ::ph::add_core_log_group(#groupName, #category);\
	\
		return ::ph::detail::CORE_LOGGER();\
	}

#define PH_LOG_TO_CORE_LOGGER(groupName, logLevel, formatStrings, ...)\
	internal_impl_logger_access_##groupName##().log(logLevel, );
