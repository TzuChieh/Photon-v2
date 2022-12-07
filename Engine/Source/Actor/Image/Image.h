#pragma once

#include "DataIO/SDL/TSdlResourceBase.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/Texture/TTexture.h"
#include "Math/Color/Spectrum.h"
#include "Common/config.h"
#include "DataIO/SDL/sdl_interface.h"
#include "Math/TArithmeticArray.h"

#include <memory>

namespace ph { class ActorCookingContext; }

namespace ph
{

class Image : public TSdlResourceBase<ETypeCategory::Ref_Image>
{
public:
	using Array = math::TArithmeticArray<float64, PH_NUMERIC_IMAGE_MAX_ELEMENTS>;
	
	inline static constexpr auto ARRAY_SIZE = Array::NUM_ELEMENTS;

public:
	Image();

	virtual std::shared_ptr<TTexture<Image::Array>> genNumericTexture(
		ActorCookingContext& ctx) = 0;

	virtual std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		ActorCookingContext& ctx) = 0;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<Image>)
	{
		ClassType clazz("image");
		clazz.docName("Image");
		clazz.description("A block of data.");
		return clazz;
	}
};

}// end namespace ph
