#include "DataIO/EPictureFile.h"

namespace ph
{

EPictureFile picture_file_type_from_extension(std::string_view ext)
{
	if(ext == ".exr" || ext == ".EXR")
	{
		return EPictureFile::EXR;
	}
	else if(ext == ".hdr" || ext == ".HDR")
	{
		return EPictureFile::HDR;
	}
	else if(ext == ".pfm" || ext == ".PFM")
	{
		return EPictureFile::PFM;
	}
	else if(ext == ".png" || ext == ".PNG")
	{
		return EPictureFile::PNG;
	}
	else if(ext == ".jpg" || ext == ".JPG")
	{
		return EPictureFile::JPG;
	}
	else if(ext == ".bmp" || ext == ".BMP")
	{
		return EPictureFile::BMP;
	}
	else if(ext == ".tga" || ext == ".TGA")
	{
		return EPictureFile::TGA;
	}
	else
	{
		return EPictureFile::Unknown;
	}
}

}// end namespace ph
