#include "DataIO/io_utils.h"
#include "DataIO/io_exceptions.h"
#include "Common/assertion.h"
#include "DataIO/PictureLoader.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace ph::io_utils
{

std::string load_text(const Path& filePath)
{
	std::ifstream textFile;
	textFile.open(filePath.toAbsoluteString());
	if(!textFile.is_open())
	{
		throw FileIOError("cannot open text file <" + filePath.toAbsoluteString() + ">");
	}

	// OPTIMIZATION: a redundant copy here
	std::stringstream buffer;
	buffer << textFile.rdbuf();
	return buffer.str();
}

// TODO: move impl here
HdrRgbFrame load_picture(const Path& picturePath)
{
	return PictureLoader::load(picturePath);
}

// TODO: move impl here
LdrRgbFrame load_LDR_picture(const Path& picturePath)
{
	return PictureLoader::loadLdr(picturePath);
}

// TODO: move impl here
HdrRgbFrame load_HDR_picture(const Path& picturePath)
{
	return PictureLoader::loadHdr(picturePath);
}

bool has_LDR_support(const std::string_view filenameExt)
{
	return 
		filenameExt == ".png"  || filenameExt == ".PNG"  ||
		filenameExt == ".jpg"  || filenameExt == ".JPG"  ||
		filenameExt == ".jpeg" || filenameExt == ".JPEG" ||
		filenameExt == ".bmp"  || filenameExt == ".BMP"  ||
		filenameExt == ".tga"  || filenameExt == ".TGA"  ||
		filenameExt == ".ppm"  || filenameExt == ".PPM"  ||
		filenameExt == ".pgm"  || filenameExt == ".PGM";
}

bool has_HDR_support(const std::string_view filenameExt)
{
	return
		filenameExt == ".exr" || filenameExt == ".EXR" ||
		filenameExt == ".hdr" || filenameExt == ".HDR";
}

}// end namespace ph::io_utils
