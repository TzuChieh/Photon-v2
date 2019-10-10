#pragma once

#include "Common/primitive_type.h"
#include "Actor/Image/PictureImage.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Frame/TFrame.h"

namespace ph
{

class InputPacket;

class HdrPictureImage final : public PictureImage, public TCommandInterface<HdrPictureImage>
{
public:
	HdrPictureImage();
	explicit HdrPictureImage(const HdrRgbFrame& picture);

	std::shared_ptr<TTexture<SpectralStrength>> genTextureSpectral(
		CookingContext& context) const override;

	void setPicture(const HdrRgbFrame& picture);
	void setPicture(HdrRgbFrame&& picture);

private:
	HdrRgbFrame m_picture;

// command interface
public:
	explicit HdrPictureImage(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph
