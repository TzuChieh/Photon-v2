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
	@param referenceEngineDir A path to any directory inside the engine installation. If provided,
	this will be used as a reference point to find the standard location.
	@exception FilesystemError If the standard location is not found.
	*/
	static EngineInitSettings loadStandardConfig(const std::string& referenceEngineDir = "");

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

	/*!
	If left empty, the engine will deduce the installation directory based on the current executable.
	If the engine is being linked by an executable not located at this project's standard binary directory,
	this option can be used to specify the installation directory for the engine.
	*/
	std::string installationDirectory = "";

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
