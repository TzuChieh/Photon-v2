#pragma once

#include "Common/primitive_type.h"
#include "Actor/Image/PictureImage.h"
#include "Frame/TFrame.h"

namespace ph
{

class InputPacket;

class LdrPictureImage final : public PictureImage
{
public:
	LdrPictureImage();
	explicit LdrPictureImage(const LdrRgbFrame& picture);

	std::shared_ptr<TTexture<Spectrum>> genTextureSpectral(
		CookingContext& context) const override;

	void setPicture(const LdrRgbFrame& picture);
	void setPicture(LdrRgbFrame&& picture);

private:
	LdrRgbFrame m_picture;
};

}// end namespace ph
