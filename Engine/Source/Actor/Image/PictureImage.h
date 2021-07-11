#pragma once

#include "Actor/Image/Image.h"

namespace ph
{

enum class EImgSampleMode
{
	NEAREST,
	BILINEAR,
	MIPMAP_TRILINEAR
};

enum class EImgWrapMode
{
	REPEAT,
	CLAMP_TO_EDGE
};

class PictureImage : public Image
{
public:
	PictureImage();

	std::shared_ptr<TTexture<Spectrum>> genTextureSpectral(
		ActorCookingContext& ctx) const override = 0;

	EImgSampleMode getSampleMode() const;
	EImgWrapMode   getWrapMode() const;

	PictureImage& setSampleMode(EImgSampleMode mode);
	PictureImage& setWrapMode(EImgWrapMode mode);

private:
	EImgSampleMode m_sampleMode;
	EImgWrapMode   m_wrapMode;
};

}// end namespace ph
