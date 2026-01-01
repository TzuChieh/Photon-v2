#pragma once

#include "Engine/DataIO/EPictureFile.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

PH_DEFINE_SDL_ENUM(EPictureFile, e)
{
	e.name("picture-file");
	e.description("File format of picture files.");

	e.addEntry(EnumType::Unknown, "");
	e.addEntry(EnumType::PNG, "PNG");
	e.addEntry(EnumType::JPG, "JPG");
	e.addEntry(EnumType::BMP, "BMP");
	e.addEntry(EnumType::TGA, "TGA");
	e.addEntry(EnumType::HDR, "HDR");
	e.addEntry(EnumType::EXR, "EXR");
	e.addEntry(EnumType::HighPrecisionEXR, "EXR-high-precision");
	e.addEntry(EnumType::PFM, "PFM");

	// Aliases
	e.addEntry(EnumType::PNG, "png");
	e.addEntry(EnumType::JPG, "jpg");
	e.addEntry(EnumType::BMP, "bmp");
	e.addEntry(EnumType::TGA, "tga");
	e.addEntry(EnumType::HDR, "hdr");
	e.addEntry(EnumType::EXR, "exr");
	e.addEntry(EnumType::HighPrecisionEXR, "exr-high-precision");
	e.addEntry(EnumType::PFM, "pfm");
}

}// end namespace ph
