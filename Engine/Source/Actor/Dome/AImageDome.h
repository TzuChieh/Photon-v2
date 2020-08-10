#pragma once

#include "Actor/ADome.h"
#include "DataIO/FileSystem/Path.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/Spectrum.h"

#include <memory>

namespace ph
{

/*! @brief Model the sky of the scene with an image.

Using a background image to represent the energy emitted from far away.
*/
class AImageDome : public ADome, public TCommandInterface<AImageDome>
{
public:
	AImageDome();
	explicit AImageDome(const Path& imagePath);
	AImageDome(const AImageDome& other);

	std::shared_ptr<TTexture<Spectrum>> loadRadianceFunction(CookingContext& context) override;
	math::Vector2S getResolution() const override;

	AImageDome& operator = (AImageDome rhs);

	friend void swap(AImageDome& first, AImageDome& second);

private:
	Path           m_imagePath;
	math::Vector2S m_imageResolution;

// command interface
public:
	explicit AImageDome(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  actor      </category>
	<type_name> image-dome </type_name>
	<extend>    actor.dome </extend>

	<name> Image Dome Actor </name>
	<description>
		Using a background image to represent the energy emitted from far away.
	</description>

	<command type="creator">
		<input name="image" type="string">
			<description>
				Resource identifier for an image describing the energy distribution.
			</description>
		</input>
	</command>

	</SDL_interface>
*/
