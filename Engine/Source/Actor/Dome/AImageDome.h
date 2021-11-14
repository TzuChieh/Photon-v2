#pragma once

#include "Actor/ADome.h"
#include "DataIO/FileSystem/Path.h"
#include "Core/Texture/TTexture.h"
#include "Math/Color/Spectrum.h"
#include "DataIO/SDL/sdl_interface.h"

#include <memory>

namespace ph
{

/*! @brief Model the sky of the scene with an image.

Using a background image to represent the energy emitted from far away.
*/
class AImageDome : public ADome
{
public:
	AImageDome();
	explicit AImageDome(const Path& imagePath);
	AImageDome(const AImageDome& other) = default;

	std::shared_ptr<TTexture<math::Spectrum>> loadRadianceFunction(ActorCookingContext& ctx) override;
	math::Vector2S getResolution() const override;

	AImageDome& operator = (AImageDome rhs);

	friend void swap(AImageDome& first, AImageDome& second);

private:
	Path           m_imagePath;
	math::Vector2S m_imageResolution;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<AImageDome>)
	{
		ClassType clazz("image-dome");
		clazz.docName("Image Dome Actor");
		clazz.description("Using a background image to represent the energy emitted from far away.");
		clazz.baseOn<ADome>();

		TSdlPath<OwnerType> imagePath("image", &OwnerType::m_imagePath);
		imagePath.description("Resource identifier for an image describing the energy distribution.");
		imagePath.required();
		clazz.addField(imagePath);

		return clazz;
	}
};

}// end namespace ph
