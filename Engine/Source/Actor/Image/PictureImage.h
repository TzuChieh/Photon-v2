#pragma once

#include "Actor/Image/Image.h"
#include "DataIO/SDL/TCommandInterface.h"

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

	std::shared_ptr<TTexture<SpectralStrength>> genTextureSpectral(
		CookingContext& context) const override = 0;

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

/*
	<SDL_interface>

	<category>  image       </category>
	<type_name> picture     </type_name>
	<extend>    image.image </extend>

	<name> Picture Image </name>
	<description>
		This kind of image is similar to ordinary color image formats.
	</description>

	<command type="creator" intent="blueprint">
		<input name="sample-mode" type="string">
			<description>
				Controls how the image will be sampled. "nearest": nearest sampling, fast but 
				blocky at close distances; "bilinear": bilinearly interpolated sampling, a good
				trade-off between speed and quality.
			</description>
		</input>
		<input name="wrap-mode" type="string">
			<description>
				Controls how the image will be sampled when normalized texture coordinates is not 
				within the range [0, 1]. "repeat": the image will repeat itself, connecting side-
				by-side with each other; "clamp-to-edge": use the border color for all 
				out-of-range coordinates.
			</description>
		</input>
	</command>

	</SDL_interface>
*/
