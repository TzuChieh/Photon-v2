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

}// end namespace ph
