#pragma once

#include "Common/primitive_type.h"
#include "Actor/Image/PictureImage.h"
#include "Frame/TFrame.h"

namespace ph
{

class HdrPictureImage final : public PictureImage
{
public:
	HdrPictureImage();
	explicit HdrPictureImage(const HdrRgbFrame& picture);

	std::shared_ptr<TTexture<Spectrum>> genTextureSpectral(
		ActorCookingContext& ctx) const override;

	void setPicture(const HdrRgbFrame& picture);
	void setPicture(HdrRgbFrame&& picture);

private:
	HdrRgbFrame m_picture;
};

}// end namespace ph
