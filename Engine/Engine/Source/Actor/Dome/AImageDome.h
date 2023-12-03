#pragma once

#include "Actor/ADome.h"
#include "DataIO/FileSystem/ResourceIdentifier.h"
#include "Core/Texture/TTexture.h"
#include "Math/Color/Spectrum.h"
#include "SDL/sdl_interface.h"

#include <memory>

namespace ph
{

/*! @brief Model the sky of the scene with an image.
Using a background image to represent the energy emitted from far away.
*/
class AImageDome : public ADome
{
public:
	std::shared_ptr<TTexture<math::Spectrum>> loadRadianceFunction(
		const CookingContext& ctx, DomeRadianceFunctionInfo* out_info) const override;

private:
	ResourceIdentifier m_imageFile;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<AImageDome>)
	{
		ClassType clazz("image-dome");
		clazz.docName("Image Dome Actor");
		clazz.description("Using a background image to represent the energy emitted from far away.");
		clazz.baseOn<ADome>();

		TSdlResourceIdentifier<OwnerType> imageFile("image-file", &OwnerType::m_imageFile);
		imageFile.description("An image describing the energy distribution.");
		imageFile.required();
		clazz.addField(imageFile);

		return clazz;
	}
};

}// end namespace ph
