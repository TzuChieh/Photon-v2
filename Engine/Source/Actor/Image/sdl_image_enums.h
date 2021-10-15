#pragma once

#include "Actor/Image/image_enums.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(TBasicSdlEnum<EImageSampleMode>)
{
	SdlEnumType sdlEnum("sample-mode");
	sdlEnum.description("Controls how the image will be sampled.");

	sdlEnum.addEntry(EnumType::UNSPECIFIED, "");
	sdlEnum.addEntry(EnumType::Nearest,     "nearest");
	sdlEnum.addEntry(EnumType::Bilinear,    "bilinear");
	sdlEnum.addEntry(EnumType::Trilinear,   "trilinear");

	return sdlEnum;
}

PH_DEFINE_SDL_ENUM(TBasicSdlEnum<EImageWrapMode>)
{
	SdlEnumType sdlEnum("wrap-mode");
	sdlEnum.description(
		"Controls how the image will be sampled when texture coordinates is not within the range [0, 1].");

	sdlEnum.addEntry(EnumType::UNSPECIFIED, "");
	sdlEnum.addEntry(EnumType::Repeat,      "repeat");
	sdlEnum.addEntry(EnumType::ClampToEdge, "clamp-to-edge");

	return sdlEnum;
}

}// end namespace ph
