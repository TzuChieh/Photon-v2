#pragma once

#include "Engine/Core/SurfaceBehavior/Property/enums.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

enum class EInterfaceFresnel
{
	Unspecified = 0,

	Schlick,
	Exact
};

PH_DEFINE_SDL_ENUM(EInterfaceFresnel, e)
{
	e.name("interface-fresnel");
	e.description("Controls the Fresnel model used.");

	e.addEntry(EnumType::Unspecified, "");
	e.addEntry(EnumType::Schlick, "schlick", "An approximative model developed by Schlick.");
	e.addEntry(EnumType::Exact, "exact", "The full-form Fresnel formula.");
}

enum class EInterfaceMicrosurface
{
	Unspecified = 0,

	TrowbridgeReitz,// a.k.a. GGX
	Beckmann
};

PH_DEFINE_SDL_ENUM(EInterfaceMicrosurface, e)
{
	e.name("interface-microsurface");
	e.description("Controls the model for describing the micro structure of the interface.");

	e.addEntry(EnumType::Unspecified, "");
	e.addEntry(EnumType::TrowbridgeReitz, "ggx", "Formally known as the Trowbridge-Reitz distribution.");
	e.addEntry(EnumType::Beckmann, "beckmann", "The Beckmann distribution.");
}

enum class ERoughnessToAlpha
{
	Unspecified = 0,

	Equaled,
	Squared,
	PbrtV3
};

PH_DEFINE_SDL_ENUM(ERoughnessToAlpha, e)
{
	e.name("roughness-to-alpha");
	e.description(
		"How roughness value will be mapped to alpha, a value that controls "
		"surface normal distribution function.");

	e.addEntry(EnumType::Unspecified, "");

	e.addEntry(EnumType::Equaled, "equaled",
		"Directly assign roughness value as-is to alpha.");

	e.addEntry(EnumType::Squared, "squared",
		"Mapping for a perceptually linear roughness. According to a course note in SIGGRAPH 2014: "
		"Moving Frostbite to Physically Based Rendering 3.0, P.68, they concluded that a squared "
		"mapping gives slightly better distribution of the profiles (blur amount) among all mip "
		"levels in the case of pre-integrated diffuse IBL maps.");

	e.addEntry(EnumType::PbrtV3, "pbrt-v3",
		"The mapping used in PBRT-v3.");
}

PH_DEFINE_SDL_ENUM(EMaskingShadowing, e)
{
	e.name("masking-shadowing");
	e.description(
		"The type of masking and shadowing term for a microfacet distribution.");

	e.addEntry(EnumType::HightCorrelated, "");

	e.addEntry(EnumType::HightCorrelated, "height-correlated",
		"Modeling the correlation between masking and shadowing due to the height of the microsurface.");

	e.addEntry(EnumType::Separable, "separable",
		"Statistically independent masking and shadowing.");

	e.addEntry(EnumType::DirectionCorrelated, "dir-correlated",
		"Modeling the correlation between masking and shadowing due to the differences between "
		"incident and outgoing directions.");

	e.addEntry(EnumType::HeightDirectionCorrelated, "height-dir-correlated",
		"Modeling the correlation between masking and shadowing due to both height and direction "
		"differences.");
}

PH_DEFINE_SDL_ENUM(ENormalMapFormat, e)
{
	e.name("normal-map-format");
	e.description("Controls how RGB values in a normal map are decoded.");

	e.addEntry(EnumType::PXPYPZ_8Bits, "opengl",
		"The conventional OpenGL format, where RGB stores +x, +y, +z.");

	e.addEntry(EnumType::PXNYPZ_8Bits, "directx",
		"The conventional DirectX format, where RGB stores +x, -y, +z.");
}
}// end namespace ph
