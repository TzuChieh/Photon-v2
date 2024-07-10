#pragma once

#include "ESurfaceRefineMode.h"

#include <Common/Log/logger_fwd.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <vector>
#include <string>

namespace ph
{

/*! @brief Options for initializing core engine.
These settings are loaded on engine startup and remains constant till engine exit. Changing the
settings will require an engine restart to see the effect.
*/
class EngineInitSettings final
{
public:
	/*! @brief Load from a standard location. Create one if the file does not exist.
	*/
	static EngineInitSettings loadStandardConfig();

	/*! @brief Additional log handlers for the core engine's internal logger.
	Important note: Handler may be called concurrently hence its implementation must ensure proper
	thread-safety. It is also inadvisable to perform time consuming tasks in the handler.
	*/
	std::vector<LogHandler> additionalLogHandlers;

	ESurfaceRefineMode surfaceRefineMode = ESurfaceRefineMode::Default;

	real selfIntersectDelta = 0.0002_r;
	std::size_t numIterativeSurfaceRefinements = 1;

	/*! @brief Randomly seed the engine RNGs, so the output is not deterministic.
	This is useful if multiple machines are working on the same scene using a mergeable algorithm.
	*/
	bool useRandomSeed = false;

	uint32 fixedSeed = 42;
	uint32 fixedSeedStep = 1;

	/*! @brief Tries to load from a saved file.
	If the loading failed, settings are not changed; othersie, settings will be updated to the values
	saved in the file.
	@return Whether the load was successful.
	*/
	bool tryLoad(const std::string& settingsFile);

	void load(const std::string& settingsFile);
	void save(const std::string& settingsFile) const;
};

}// end namespace ph
