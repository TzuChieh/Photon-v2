#pragma once

#include "Core/SurfaceBehavior/Property/enums.h"
#include "SDL/sdl_interface.h"

namespace ph
{

enum class EInterfaceFresnel
{
	Unspecified = 0,

	Schlick,
	Exact
};

enum class EInterfaceMicrosurface
{
	Unspecified = 0,

	TrowbridgeReitz,// a.k.a. GGX
	Beckmann
};

enum class ERoughnessToAlpha
{
	Unspecified = 0,

	Equaled,
	Squared,
	PbrtV3
};

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<EInterfaceFresnel>)
{
	SdlEnumType sdlEnum("interface-fresnel");
	sdlEnum.description("Controls the Fresnel model used.");

	sdlEnum.addEntry(EnumType::Unspecified, "");
	sdlEnum.addEntry(EnumType::Schlick, "schlick", "An approximative model developed by Schlick.");
	sdlEnum.addEntry(EnumType::Exact, "exact", "The full-form Fresnel formula.");

	return sdlEnum;
}

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<EInterfaceMicrosurface>)
{
	SdlEnumType sdlEnum("interface-microsurface");
	sdlEnum.description("Controls the model for describing the micro structure of the interface.");

	sdlEnum.addEntry(EnumType::Unspecified, "");
	sdlEnum.addEntry(EnumType::TrowbridgeReitz, "ggx", "Formally known as the Trowbridge-Reitz distribution.");
	sdlEnum.addEntry(EnumType::Beckmann, "beckmann", "The Beckmann distribution.");

	return sdlEnum;
}

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

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<EMaskingShadowing>)
{
	SdlEnumType sdlEnum("masking-shadowing");
	sdlEnum.description(
		"The type of masking and shadowing term for a microfacet distribution.");

	sdlEnum.addEntry(EnumType::HightCorrelated, "");

	sdlEnum.addEntry(EnumType::HightCorrelated, "height-correlated",
		"Modeling the correlation between masking and shadowing due to the height of the microsurface.");

	sdlEnum.addEntry(EnumType::Separable, "separable",
		"Statistically independent masking and shadowing.");

	sdlEnum.addEntry(EnumType::DirectionCorrelated, "dir-correlated",
		"Modeling the correlation between masking and shadowing due to the differences between "
		"incident and outgoing directions.");

	sdlEnum.addEntry(EnumType::HeightDirectionCorrelated, "height-dir-correlated",
		"Modeling the correlation between masking and shadowing due to both height and direction "
		"differences.");

	return sdlEnum;
}

}// end namespace ph
