#pragma once

#include "Common/ELogLevel.h"
#include "Common/config.h"

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

	std::size_t addGroup(std::string_view groupName, std::string_view category = "");
	std::size_t numGroups() const;
	const LogGroup& getGroup(std::size_t index) const;

private:
	std::vector<LogGroup> m_groups;
};

LogGroups get_core_log_groups();

}// end namespace ph

namespace ph::detail::core_logging
{

Logger& CORE_LOGGER();
std::size_t add_log_group(std::string_view groupName, std::string_view category = "");
void log_to_logger(Logger& logger, std::string_view groupName, ELogLevel logLevel, std::string_view logMessage);

}// end namespace ph::detail::core_logging

#define PH_DECLARE_LOG_GROUP(groupName)\
	::ph::Logger& internal_impl_logger_access_##groupName()

#define PH_DEFINE_LOG_GROUP(groupName, category)\
	::ph::Logger& internal_impl_logger_access_##groupName()\
	{\
		static const std::size_t logGroupIndex = ::ph::detail::core_logging::add_log_group(#groupName, #category);\
	\
		return ::ph::detail::core_logging::CORE_LOGGER();\
	}

#define PH_DEFINE_INLINE_LOG_GROUP(groupName, category)\
	inline ::ph::Logger& internal_impl_logger_access_##groupName()\
	{\
		static const std::size_t logGroupIndex = ::ph::detail::core_logging::add_log_group(#groupName, #category);\
	\
		return ::ph::detail::core_logging::CORE_LOGGER();\
	}

#define PH_DEFINE_INTERNAL_LOG_GROUP(groupName, category)\
	namespace\
	{\
		PH_DEFINE_INLINE_LOG_GROUP(groupName, category);\
	}

#define PH_DEFINE_EXTERNAL_LOG_GROUP(groupName, category) PH_DEFINE_INLINE_LOG_GROUP(groupName, category)

#define PH_LOG_RAW_STRING_TO_CORE_LOGGER(groupName, logLevel, rawString)\
	::ph::detail::core_logging::log_to_logger(\
		internal_impl_logger_access_##groupName(),\
		#groupName,\
		logLevel,\
		rawString)

// TODO: it could be beneficial to determine when can we use std::vformat()
// instead of always using std::format() for logging
// PH_LOG_STRING() variant for directly logging a runtime string?

#define PH_LOG_FORMAT_STRING_TO_CORE_LOGGER(groupName, logLevel, formatString, ...)\
	PH_LOG_RAW_STRING_TO_CORE_LOGGER(\
		groupName,\
		logLevel,\
		std::format(formatString __VA_OPT__(,) __VA_ARGS__))

#ifdef PH_ENABLE_DEBUG_LOG
	#define PH_LOG_DEBUG_STRING(groupName, rawString) PH_LOG_RAW_STRING_TO_CORE_LOGGER(groupName, ::ph::ELogLevel::Debug, rawString)
	#define PH_LOG_DEBUG(groupName, formatString, ...) PH_LOG_FORMAT_STRING_TO_CORE_LOGGER(groupName, ::ph::ELogLevel::Debug, formatString, __VA_ARGS__)
#else
	#define PH_LOG_DEBUG_STRING(groupName, rawString)
	#define PH_LOG_DEBUG(groupName, formatString, ...)
#endif

#define PH_LOG_STRING(groupName, rawString) PH_LOG_RAW_STRING_TO_CORE_LOGGER(groupName, ::ph::ELogLevel::Note, rawString)
#define PH_LOG(groupName, formatString, ...) PH_LOG_FORMAT_STRING_TO_CORE_LOGGER(groupName, ::ph::ELogLevel::Note, formatString, __VA_ARGS__)

#define PH_LOG_WARNING_STRING(groupName, rawString) PH_LOG_RAW_STRING_TO_CORE_LOGGER(groupName, ::ph::ELogLevel::Warning, rawString)
#define PH_LOG_WARNING(groupName, formatString, ...) PH_LOG_FORMAT_STRING_TO_CORE_LOGGER(groupName, ::ph::ELogLevel::Warning, formatString, __VA_ARGS__)

#define PH_LOG_ERROR_STRING(groupName, rawString) PH_LOG_RAW_STRING_TO_CORE_LOGGER(groupName, ::ph::ELogLevel::Error, rawString)
#define PH_LOG_ERROR(groupName, formatString, ...) PH_LOG_FORMAT_STRING_TO_CORE_LOGGER(groupName, ::ph::ELogLevel::Error, formatString, __VA_ARGS__)
