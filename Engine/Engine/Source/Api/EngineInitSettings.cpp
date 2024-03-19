#include "EngineInitSettings.h"

#include <Common/config.h>
#include <Common/io_exceptions.h>
#include <Common/Config/IniFile.h>
#include <Common/Utility/string_utils.h>

#include <filesystem>

namespace ph
{

EngineInitSettings EngineInitSettings::loadStandardConfig()
{
	auto const filePath = PH_CONFIG_DIRECTORY "EngineConfig.ini";

	EngineInitSettings settings;
	if(!settings.tryLoad(filePath))
	{
		settings.save(filePath);
	}

	return settings;
}

bool EngineInitSettings::tryLoad(const std::string& settingsFile)
{
	const std::filesystem::path filePath(settingsFile);
	if(!std::filesystem::is_regular_file(filePath) ||
	   (filePath.extension() != ".ini" && filePath.extension() != ".INI"))
	{
		return false;
	}

	load(settingsFile);
	return true;
}

void EngineInitSettings::load(const std::string& settingsFile)
{
	IniFile file(settingsFile);

	if(auto optIdx = file.findPropertyIndex("RefineMode"); optIdx)
	{
		surfaceRefineMode = str_to_refine_mode(file.getPropertyValue(*optIdx));
	}

	if(auto optIdx = file.findPropertyIndex("SelfIntersectDelta"); optIdx)
	{
		selfIntersectDelta = string_utils::parse_number<real>(file.getPropertyValue(*optIdx));
	}

	if(auto optIdx = file.findPropertyIndex("NumIterativeSurfaceRefinements"); optIdx)
	{
		numIterativeSurfaceRefinements = string_utils::parse_number<std::size_t>(file.getPropertyValue(*optIdx));
	}

	if(auto optIdx = file.findPropertyIndex("UseRandomSeed"); optIdx)
	{
		useRandomSeed = string_utils::parse_number<bool>(file.getPropertyValue(*optIdx));
	}

	if(auto optIdx = file.findPropertyIndex("FixedSeed"); optIdx)
	{
		fixedSeed = string_utils::parse_number<uint32>(file.getPropertyValue(*optIdx));
	}

	if(auto optIdx = file.findPropertyIndex("FixedSeedStep"); optIdx)
	{
		fixedSeedStep = string_utils::parse_number<uint32>(file.getPropertyValue(*optIdx));
	}
}

void EngineInitSettings::save(const std::string& settingsFile) const
{
	IniFile file;

	file.setProperty("RefineMode", refine_mode_to_str(surfaceRefineMode));
	file.setProperty("SelfIntersectDelta", string_utils::stringify_number(selfIntersectDelta));
	file.setProperty("NumIterativeSurfaceRefinements", string_utils::stringify_number(numIterativeSurfaceRefinements));
	file.setProperty("UseRandomSeed", string_utils::stringify_number(useRandomSeed));
	file.setProperty("FixedSeed", string_utils::stringify_number(fixedSeed));
	file.setProperty("FixedSeedStep", string_utils::stringify_number(fixedSeedStep));

	file.save(settingsFile);
}

}// end namespace ph
