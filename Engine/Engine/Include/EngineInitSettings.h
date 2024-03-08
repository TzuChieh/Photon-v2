#pragma once

#include "ESurfaceRefineMode.h"

#include <Common/Log/logger_fwd.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <vector>

namespace ph
{

/*! @brief Options for initializing core engine.
These settings are loaded on engine startup and remains constant till engine exit. Changing the
settings will require an engine restart to see the effect.
*/
class EngineInitSettings
{
public:
	/*! @brief Additional log handlers for the core engine's internal logger.
	Important note: Handler may be called concurrently hence its implementation must ensure proper
	thread-safety. It is also inadvisable to perform time consuming tasks in the handler.
	*/
	std::vector<LogHandler> additionalLogHandlers;

	ESurfaceRefineMode surfaceRefineMode = ESurfaceRefineMode::Default;

	real selfIntersectDelta = 0.0002_r;
	std::size_t numIterativeSurfaceRefinements = 4;
};

}// end namespace ph
