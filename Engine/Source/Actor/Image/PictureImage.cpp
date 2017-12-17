#include "Actor/Image/PictureImage.h"

namespace ph
{

PictureImage::~PictureImage() = default;

// command interface

SdlTypeInfo PictureImage::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_IMAGE, "picture");
}

void PictureImage::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph