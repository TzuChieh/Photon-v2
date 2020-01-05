#include "Actor/Image/HdrPictureImage.h"
#include "DataIO/SDL/InputPacket.h"
#include "DataIO/SDL/InputPrototype.h"
#include "DataIO/PictureLoader.h"
#include "Core/Texture/HdrRgbTexture2D.h"
#include "Core/Texture/TNearestPixelTex2D.h"
#include "Core/Texture/TBilinearPixelTex2D.h"

#include <memory>
#include <utility>

namespace ph
{

HdrPictureImage::HdrPictureImage() :
	HdrPictureImage(HdrRgbFrame())
{}

HdrPictureImage::HdrPictureImage(const HdrRgbFrame& picture) :
	PictureImage(),
	m_picture(picture)
{}

std::shared_ptr<TTexture<SpectralStrength>> HdrPictureImage::genTextureSpectral(
	CookingContext& context) const
{
	std::unique_ptr<TAbstractPixelTex2D<HdrComponent, 3>> texture;
	switch(getSampleMode())
	{
	case EImgSampleMode::NEAREST:
		texture = std::make_unique<TNearestPixelTex2D<HdrComponent, 3>>(m_picture);
		break;

	case EImgSampleMode::BILINEAR:
		texture = std::make_unique<TBilinearPixelTex2D<HdrComponent, 3>>(m_picture);
		break;

	default:
		texture = std::make_unique<TNearestPixelTex2D<HdrComponent, 3>>(m_picture);
		break;
	}

	switch(getWrapMode())
	{
	case EImgWrapMode::REPEAT: 
		texture->setWrapMode(ETexWrapMode::REPEAT); 
		break;

	case EImgWrapMode::CLAMP_TO_EDGE: 
		texture->setWrapMode(ETexWrapMode::CLAMP_TO_EDGE); 
		break;
	}

	return std::make_shared<HdrRgbTexture2D>(std::move(texture));
}

void HdrPictureImage::setPicture(const HdrRgbFrame& picture)
{
	m_picture = picture;
}

void HdrPictureImage::setPicture(HdrRgbFrame&& picture)
{
	m_picture = std::move(m_picture);
}

// command interface

HdrPictureImage::HdrPictureImage(const InputPacket& packet) :
	PictureImage(packet)
{
	const auto picturePath = packet.getStringAsPath(
		"image", Path(), DataTreatment::REQUIRED());

	m_picture = PictureLoader::loadHdr(picturePath);
}

SdlTypeInfo HdrPictureImage::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_IMAGE, "hdr-picture");
}

void HdrPictureImage::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(
		SdlLoader([](const InputPacket& packet)
		{
			return std::make_unique<HdrPictureImage>(packet);
		})
	);
}

}// end namespace ph
