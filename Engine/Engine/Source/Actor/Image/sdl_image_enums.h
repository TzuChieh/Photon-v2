#pragma once

#include "SDL/sdl_interface.h"

namespace ph
{

enum class EImageSampleMode
{
	Unspecified = 0,

	Bilinear,
	Nearest,
	Trilinear
};

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<EImageSampleMode>)
{
	SdlEnumType sdlEnum("sample-mode");
	sdlEnum.description("Controls how the image will be sampled.");

	sdlEnum.addEntry(EnumType::Unspecified, "");
	sdlEnum.addEntry(EnumType::Nearest,     "nearest");
	sdlEnum.addEntry(EnumType::Bilinear,    "bilinear");
	sdlEnum.addEntry(EnumType::Trilinear,   "trilinear");

	return sdlEnum;
}

enum class EImageWrapMode
{
	Unspecified = 0,

	ClampToEdge,
	Repeat,
	FlippedClampToEdge
};

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<EImageWrapMode>)
{
	SdlEnumType sdlEnum("wrap-mode");
	sdlEnum.description(
		"Controls how the image will be sampled when texture coordinates is not within the range [0, 1].");

	sdlEnum.addEntry(EnumType::Unspecified,        "");
	sdlEnum.addEntry(EnumType::Repeat,             "repeat");
	sdlEnum.addEntry(EnumType::ClampToEdge,        "clamp-to-edge");
	sdlEnum.addEntry(EnumType::FlippedClampToEdge, "flipped-clamp-to-edge");

	return sdlEnum;
}

}// end namespace ph
