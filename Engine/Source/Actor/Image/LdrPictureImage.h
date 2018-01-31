#pragma once

#include "Common/primitive_type.h"
#include "Actor/Image/PictureImage.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Frame/TFrame.h"

namespace ph
{

class LdrPictureImage final : public PictureImage, public TCommandInterface<LdrPictureImage>
{
public:
	LdrPictureImage(const LdrRgbFrame& picture);
	virtual ~LdrPictureImage() override;

	virtual std::shared_ptr<TTexture<SpectralStrength>> genTextureSpectral(
		CookingContext& context) const override;

private:
	LdrRgbFrame m_picture;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph