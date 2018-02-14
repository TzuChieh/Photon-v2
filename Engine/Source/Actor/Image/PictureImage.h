#pragma once

#include "Actor/Image/Image.h"
#include "FileIO/SDL/TCommandInterface.h"

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

class InputPacket;

class PictureImage : public Image, public TCommandInterface<PictureImage>
{
public:
	PictureImage();
	virtual ~PictureImage() override;

	virtual std::shared_ptr<TTexture<SpectralStrength>> genTextureSpectral(
		CookingContext& context) const = 0;

	EImgSampleMode getSampleMode() const;
	EImgWrapMode   getWrapMode() const;

	PictureImage& setSampleMode(EImgSampleMode mode);
	PictureImage& setWrapMode(EImgWrapMode mode);

private:
	EImgSampleMode m_sampleMode;
	EImgWrapMode   m_wrapMode;

// command interface
public:
	explicit PictureImage(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph