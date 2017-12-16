#include "Actor/Image/Image.h"

namespace ph
{

Image::~Image() = default;

// command interface

SdlTypeInfo Image::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_IMAGE, "image");
}

void Image::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph