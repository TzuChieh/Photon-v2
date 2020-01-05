#pragma once

#include "Common/primitive_type.h"
#include "Actor/Image/PictureImage.h"
#include "DataIO/SDL/TCommandInterface.h"
#include "Frame/TFrame.h"

namespace ph
{

class InputPacket;

class LdrPictureImage final : public PictureImage, public TCommandInterface<LdrPictureImage>
{
public:
	LdrPictureImage();
	explicit LdrPictureImage(const LdrRgbFrame& picture);

	std::shared_ptr<TTexture<SpectralStrength>> genTextureSpectral(
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

/*
	<SDL_interface>

	<category>  image       </category>
	<type_name> ldr-picture </type_name>
	<extend>    image.image </extend>

	<name> LDR Picture Image </name>
	<description>
		Low dynamic range images.
	</description>

	<command type="creator">
		<input name="image" type="string">
			<description>
				Resource identifier for a LDR image file.
			</description>
		</input>
	</command>

	</SDL_interface>
*/
