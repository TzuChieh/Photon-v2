#pragma once

#include "Actor/Image/Image.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class PictureImage : public Image, public TCommandInterface<PictureImage>
{
public:
	virtual ~PictureImage() override;

	virtual void genTextureSpectral(
		CookingContext& context,
		std::shared_ptr<TTexture<SpectralStrength>>* out_texture) const = 0;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph