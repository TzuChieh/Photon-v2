#include "Actor/Image/LdrPictureImage.h"
#include "FileIO/InputPacket.h"
#include "FileIO/PictureLoader.h"
#include "Core/Texture/LdrRgbTexture2D.h"
#include "Core/Texture/TNearestPixelTex2D.h"

#include <memory>

namespace ph
{

LdrPictureImage::LdrPictureImage(const LdrRgbFrame& picture) :
	m_picture(picture)
{

}

LdrPictureImage::~LdrPictureImage() = default;

std::shared_ptr<TTexture<SpectralStrength>> LdrPictureImage::genTextureSpectral(
	CookingContext& context) const
{
	auto nearestFilteredTexture = std::make_unique<TNearestPixelTex2D<LdrComponent, 3>>(m_picture);
	return std::make_shared<LdrRgbTexture2D>(std::move(nearestFilteredTexture));
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
			"image", Path(), DataTreatment::REQUIRED());

		return std::make_unique<LdrPictureImage>(PictureLoader::loadLdr(picturePath));
	}));
}

}// end namespace ph