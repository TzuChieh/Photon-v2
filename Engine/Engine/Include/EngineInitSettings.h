#pragma once

#include <Common/Log/logger_fwd.h>

#include <vector>

namespace ph
{

class EngineInitSettings
{
public:
	/*! @brief Additional log handlers for the core engine's internal logger.
	Important note: Handler may be called concurrently hence its implementation must ensure proper
	thread-safety. It is also inadvisable to perform time consuming tasks in the handler.
	*/
	std::vector<LogHandler> additionalLogHandlers;
};

}// end namespace ph
