#pragma once

#include "SDL/sdl_interface.h"

namespace ph
{

enum class ERoughnessToAlpha
{
	Unspecified = 0,

	Equaled,
	Squared,
	PbrtV3
};

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<ERoughnessToAlpha>)
{
	SdlEnumType sdlEnum("roughness-to-alpha");
	sdlEnum.description(
		"How roughness value will be mapped to alpha, a value that controls "
		"surface normal distribution function.");

	sdlEnum.addEntry(EnumType::Unspecified, "");

	sdlEnum.addEntry(EnumType::Equaled, "equaled",
		"Directly assign roughness value as-is to alpha.");

	sdlEnum.addEntry(EnumType::Squared, "squared",
		"Mapping for a perceptually linear roughness. According to a course note in SIGGRAPH 2014: "
		"Moving Frostbite to Physically Based Rendering 3.0, P.68, they concluded that a squared "
		"mapping gives slightly better distribution of the profiles (blur amount) among all mip "
		"levels in the case of pre-integrated diffuse IBL maps.");

	sdlEnum.addEntry(EnumType::PbrtV3, "pbrt-v3",
		"The mapping used in PBRT-v3.");

	return sdlEnum;
}

}// end namespace ph
