#include "Actor/Image/LdrPictureImage.h"
#include "DataIO/PictureLoader.h"
#include "Core/Texture/LdrRgbTexture2D.h"
#include "Core/Texture/TNearestPixelTex2D.h"
#include "Core/Texture/TBilinearPixelTex2D.h"

#include <memory>
#include <utility>

namespace ph
{

LdrPictureImage::LdrPictureImage() : 
	LdrPictureImage(LdrRgbFrame())
{}

LdrPictureImage::LdrPictureImage(const LdrRgbFrame& picture) :
	PictureImage(),
	m_picture(picture)
{}

std::shared_ptr<TTexture<math::Spectrum>> LdrPictureImage::genTextureSpectral(
	ActorCookingContext& ctx) const
{
	std::unique_ptr<TAbstractPixelTex2D<LdrComponent, 3>> texture;
	switch(getSampleMode())
	{
	case EImgSampleMode::NEAREST:
		texture = std::make_unique<TNearestPixelTex2D<LdrComponent, 3>>(m_picture);
		break;

	case EImgSampleMode::BILINEAR:
		texture = std::make_unique<TBilinearPixelTex2D<LdrComponent, 3>>(m_picture);
		break;

	default:
		texture = std::make_unique<TNearestPixelTex2D<LdrComponent, 3>>(m_picture);
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

	return std::make_shared<LdrRgbTexture2D>(std::move(texture));
}

void LdrPictureImage::setPicture(const LdrRgbFrame& picture)
{
	m_picture = picture;
}

void LdrPictureImage::setPicture(LdrRgbFrame&& picture)
{
	m_picture = std::move(m_picture);
}

}// end namespace ph
