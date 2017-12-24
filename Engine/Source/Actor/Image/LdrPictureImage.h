#pragma once

#include "Common/primitive_type.h"
#include "Actor/Image/Image.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Frame/TFrame.h"

namespace ph
{

class LdrPictureImage final : public Image, public TCommandInterface<LdrPictureImage>
{
public:
	LdrPictureImage(const TFrame<uint8>& picture);
	virtual ~LdrPictureImage() override;

	virtual void genTexture(CookingContext& context,
	                        std::shared_ptr<TTexture<SpectralStrength>>* out_texture) const override;

private:
	TFrame<uint8> m_picture;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph