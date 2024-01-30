#pragma once

/*! @file
@brief Logging functions.
*/

#include "Common/Log/logger_fwd.h"
#include "Common/Log/ELogLevel.h"
#include "Common/config.h"
#include "Common/utility.h"

#include <string>
#include <vector>
#include <cstddef>
#include <format>

/*! @file
Note on loggers:
All logging functionalities are thread-safe when using pre-defined macros. It is not advisible
to log in class dtor, especially for static instances as they may live outside of engine's lifetime.
Also make sure not to call any logging functions when the logger is not initialized.
*/

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

/*! @brief Initializes core logging functionalities.
Any logging is only valid after calling `init()`.
*/
void init();

/*! @brief Terminates core logging functionalities.
Cleanup after logging is finished.
*/
void exit();

/*! @brief Get the core logger.
@note Const methods of core logger are thread-safe.
*/
Logger& get_logger();

/*! @brief Add a log group to the core logger.
@note Thread-safe.
*/
std::size_t add_log_group(std::string_view groupName, std::string_view category = "");

/*! @brief Log information to the specified logger.
@note Thread-safe if the logger is thread-safe.
*/
void log_to_logger(const Logger& logger, std::string_view groupName, ELogLevel logLevel, std::string_view logMessage);

}// end namespace ph::detail::core_logging

/*! @brief Declares a logger.
The logger should be defined using `PH_DEFINE_LOG_GROUP()` somewhere in the source (preferably in a
.cpp file).
*/
#define PH_DECLARE_LOG_GROUP(groupName)\
	const ::ph::Logger& internal_impl_logger_access_##groupName()

/*! @brief Defines a logger.
*/
#define PH_DEFINE_LOG_GROUP(groupName, category)\
	const ::ph::Logger& internal_impl_logger_access_##groupName()\
	{\
		static const std::size_t logGroupIndex = ::ph::detail::core_logging::add_log_group(#groupName, #category);\
	\
		return ::ph::detail::core_logging::get_logger();\
	}

#define PH_DEFINE_INLINE_LOG_GROUP(groupName, category)\
	inline const ::ph::Logger& internal_impl_logger_access_##groupName()\
	{\
		static const std::size_t logGroupIndex = ::ph::detail::core_logging::add_log_group(#groupName, #category);\
	\
		return ::ph::detail::core_logging::get_logger();\
	}

/*! @brief Defines a logger that is private to a .cpp file.
Can only appear one time in a translation unit, preferably in a .cpp file. 
*/
#define PH_DEFINE_INTERNAL_LOG_GROUP(groupName, category)\
	namespace\
	{\
		PH_DEFINE_INLINE_LOG_GROUP(groupName, category);\
	}

/*! @brief Defines a public logger in a header file.
The logger will be usable anywhere that includes the header file containing this call.
*/
#define PH_DEFINE_EXTERNAL_LOG_GROUP(groupName, category) PH_DEFINE_INLINE_LOG_GROUP(groupName, category)

#define PH_LOG_RAW_STRING_TO_CORE_LOGGER(groupName, level, rawStringExpr)\
	do\
	{\
		constexpr auto logLevel = ::ph::ELogLevel::level;\
		const auto rawString = rawStringExpr;\
		if constexpr(::ph::is_once(logLevel))\
		{\
			static const bool PH_CONCAT_2(dummy, __LINE__) = [&]()\
			{\
				::ph::detail::core_logging::log_to_logger(\
					internal_impl_logger_access_##groupName(),\
					#groupName,\
					logLevel,\
					rawString);\
				return true;\
			}();\
		}\
		else\
		{\
			::ph::detail::core_logging::log_to_logger(\
				internal_impl_logger_access_##groupName(),\
				#groupName,\
				logLevel,\
				rawString);\
		}\
	} while(0)

// TODO: it could be beneficial to determine when can we use std::vformat()
// instead of always using std::format() for logging
// PH_LOG_STRING() variant for directly logging a runtime string?

#define PH_LOG_FORMAT_STRING_TO_CORE_LOGGER(groupName, level, formatString, ...)\
	PH_LOG_RAW_STRING_TO_CORE_LOGGER(\
		groupName,\
		level,\
		std::format(formatString __VA_OPT__(,) __VA_ARGS__))

#if PH_ENABLE_DEBUG_LOG
	#define PH_DEBUG_LOG_STRING(groupName, rawString) PH_LOG_RAW_STRING_TO_CORE_LOGGER(groupName, Debug, rawString)
	#define PH_DEBUG_LOG(groupName, formatString, ...) PH_LOG_FORMAT_STRING_TO_CORE_LOGGER(groupName, Debug, formatString, __VA_ARGS__)
#else
	#define PH_DEBUG_LOG_STRING(groupName, rawString) PH_NO_OP()
	#define PH_DEBUG_LOG(groupName, formatString, ...) PH_NO_OP()
#endif

#define PH_LOG_STRING(groupName, level, rawString) PH_LOG_RAW_STRING_TO_CORE_LOGGER(groupName, level, rawString)
#define PH_LOG(groupName, level, formatString, ...) PH_LOG_FORMAT_STRING_TO_CORE_LOGGER(groupName, level, formatString, __VA_ARGS__)

namespace ph
{

// Photon renderer's default log group
PH_DECLARE_LOG_GROUP(PhotonRenderer);

/*! @brief A set of helper utility macros to log using Photon renderer's default log group.
*/
///@{
#if PH_ENABLE_DEBUG_LOG
	#define PH_DEFAULT_DEBUG_LOG_STRING(rawString) PH_DEBUG_LOG_STRING(PhotonRenderer, rawString)
	#define PH_DEFAULT_DEBUG_LOG(formatString, ...) PH_DEBUG_LOG(PhotonRenderer, formatString, __VA_ARGS__)
#else
	#define PH_DEFAULT_DEBUG_LOG_STRING(rawString, ...) PH_NO_OP()
	#define PH_DEFAULT_DEBUG_LOG(formatString, ...) PH_NO_OP()
#endif

#define PH_DEFAULT_LOG_STRING(level, rawString) PH_LOG_STRING(PhotonRenderer, level, rawString)
#define PH_DEFAULT_LOG(level, formatString, ...) PH_LOG(PhotonRenderer, level, formatString, __VA_ARGS__)
///@}

}// end namespace ph
