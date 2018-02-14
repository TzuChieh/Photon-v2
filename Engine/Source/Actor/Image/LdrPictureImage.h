#pragma once

#include "Common/primitive_type.h"
#include "Actor/Image/PictureImage.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Frame/TFrame.h"

namespace ph
{

class InputPacket;

class LdrPictureImage final : public PictureImage, public TCommandInterface<LdrPictureImage>
{
public:
	LdrPictureImage();
	explicit LdrPictureImage(const LdrRgbFrame& picture);
	virtual ~LdrPictureImage() override;

	virtual std::shared_ptr<TTexture<SpectralStrength>> genTextureSpectral(
		CookingContext& context) const override;

	void setPicture(const LdrRgbFrame& picture);
	void setPicture(LdrRgbFrame&& picture);

private:
	LdrRgbFrame m_picture;

// command interface
public:
	explicit LdrPictureImage(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph