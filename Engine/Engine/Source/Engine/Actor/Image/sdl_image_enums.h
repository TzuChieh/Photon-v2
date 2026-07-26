#pragma once

#include "Engine/SDL/sdl_interface.h"

namespace ph
{

enum class EImageSampleMode
{
	Unspecified = 0,

	Bilinear,
	Nearest,
	Trilinear
};

PH_DEFINE_SDL_ENUM(EImageSampleMode, e)
{
	e.name("sample-mode");
	e.description("Controls how the image will be sampled.");

	e.addEntry(EnumType::Unspecified, "");
	e.addEntry(EnumType::Nearest,     "nearest");
	e.addEntry(EnumType::Bilinear,    "bilinear");
	e.addEntry(EnumType::Trilinear,   "trilinear");
}

enum class EImageWrapMode
{
	Unspecified = 0,

	ClampToEdge,
	Repeat,
	FlippedClampToEdge
};

PH_DEFINE_SDL_ENUM(EImageWrapMode, e)
{
	e.name("wrap-mode");
	e.description(
		"Controls how the image will be sampled when texture coordinates is not within the range [0, 1].");

	e.addEntry(EnumType::Unspecified,        "");
	e.addEntry(EnumType::Repeat,             "repeat");
	e.addEntry(EnumType::ClampToEdge,        "clamp-to-edge");
	e.addEntry(EnumType::FlippedClampToEdge, "flipped-clamp-to-edge");
}

enum class ENoiseAlgorithm
{
	PerlinFbm = 0
};

PH_DEFINE_SDL_ENUM(ENoiseAlgorithm, e)
{
	e.name("noise-algorithm");
	e.description("The algorithm used to generate procedural noise.");

	e.addEntry(EnumType::PerlinFbm, "perlin-fbm");
}

enum class ENoiseDimension
{
	D1 = 1,
	D2 = 2,
	D3 = 3,
	D4 = 4
};

PH_DEFINE_SDL_ENUM(ENoiseDimension, e)
{
	e.name("noise-dimension");
	e.description("The dimensionality of the procedural noise domain.");

	e.addEntry(EnumType::D1, "1d");
	e.addEntry(EnumType::D2, "2d");
	e.addEntry(EnumType::D3, "3d");
	e.addEntry(EnumType::D4, "4d");
}

enum class EAttributeKind
{
	/*! Geometry-bound UVW, or (0.5, 0.5, 0.5) when required geometry data is unavailable. */
	UvwFromGeometryBound = 0,

	/*! Geometry-local hit position, or zero when a surface hit is unavailable. */
	GeometryHitPosition = 1
};

PH_DEFINE_SDL_ENUM(EAttributeKind, e)
{
	e.name("attribute-kind");
	e.description("Data exposed by an attribute image.");

	e.addEntry(EnumType::UvwFromGeometryBound, "uvw-from-geometry-bound");
	e.addEntry(EnumType::GeometryHitPosition, "geometry-hit-position");
}

}// end namespace ph
