#include "Actor/Image/LdrPictureImage.h"
#include "FileIO/InputPacket.h"
#include "FileIO/PictureLoader.h"
#include "Core/Texture/LdrRgbTexture2D.h"

namespace ph
{

LdrPictureImage::LdrPictureImage(const TFrame<uint8>& picture) : 
	m_picture(picture)
{

}

LdrPictureImage::~LdrPictureImage() = default;

void LdrPictureImage::genTexture(
	CookingContext& context,
	std::shared_ptr<TTexture<SpectralStrength>>* const out_texture) const
{
	auto texture = std::make_shared<LdrRgbTexture2D>();
	texture->setPixels(m_picture);
	*out_texture = texture;
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