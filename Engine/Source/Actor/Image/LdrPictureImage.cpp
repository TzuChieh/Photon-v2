#include "Actor/Image/LdrPictureImage.h"
#include "FileIO/InputPacket.h"
#include "FileIO/PictureLoader.h"
#include "Core/Texture/RgbPixelTexture.h"

namespace ph
{

LdrPictureImage::LdrPictureImage(const TFrame<uint8>& picture) : 
	m_picture(picture)
{

}

LdrPictureImage::~LdrPictureImage() = default;

std::shared_ptr<Texture> LdrPictureImage::genTexture(CookingContext& context) const
{
	std::shared_ptr<RgbPixelTexture> texture;
	texture->setPixels(m_picture);
	return texture;
}

// command interface

SdlTypeInfo LdrPictureImage::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_IMAGE, "ldr-picture");
}

void LdrPictureImage::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		const Path& picturePath = packet.getStringAsPath(
			"filename", Path(), DataTreatment::REQUIRED());

		return std::make_unique<LdrPictureImage>(PictureLoader::loadLdr(picturePath));
	}));
}

}// end namespace ph