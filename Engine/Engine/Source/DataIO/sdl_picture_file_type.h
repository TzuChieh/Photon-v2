#pragma once

#include "DataIO/EPictureFile.h"
#include "SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<EPictureFile>)
{
	SdlEnumType sdlEnum("picture-file");
	sdlEnum.description("File format of picture files.");

	sdlEnum.addEntry(EnumType::Unknown, "");
	sdlEnum.addEntry(EnumType::PNG, "PNG");
	sdlEnum.addEntry(EnumType::JPG, "JPG");
	sdlEnum.addEntry(EnumType::BMP, "BMP");
	sdlEnum.addEntry(EnumType::TGA, "TGA");
	sdlEnum.addEntry(EnumType::HDR, "HDR");
	sdlEnum.addEntry(EnumType::EXR, "EXR");
	sdlEnum.addEntry(EnumType::HighPrecisionEXR, "EXR-high-precision");
	sdlEnum.addEntry(EnumType::PFM, "PFM");

	// Aliases
	sdlEnum.addEntry(EnumType::PNG, "png");
	sdlEnum.addEntry(EnumType::JPG, "jpg");
	sdlEnum.addEntry(EnumType::BMP, "bmp");
	sdlEnum.addEntry(EnumType::TGA, "tga");
	sdlEnum.addEntry(EnumType::HDR, "hdr");
	sdlEnum.addEntry(EnumType::EXR, "exr");
	sdlEnum.addEntry(EnumType::HighPrecisionEXR, "exr-high-precision");
	sdlEnum.addEntry(EnumType::PFM, "pfm");

	return sdlEnum;
}

}// end namespace ph
