#pragma once

#include "SDL/TSdlResourceBase.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/Texture/TTexture.h"
#include "Math/Color/Spectrum.h"
#include "Common/config.h"
#include "SDL/sdl_interface.h"
#include "Math/TArithmeticArray.h"

#include <memory>

namespace ph { class CookingContext; }

namespace ph
{

class Image : public TSdlResourceBase<ESdlTypeCategory::Ref_Image>
{
public:
	using ArrayType = math::TArithmeticArray<float64, PH_NUMERIC_IMAGE_MAX_ELEMENTS>;
	
	inline static constexpr auto ARRAY_SIZE = ArrayType::NUM_ELEMENTS;

public:
	virtual std::shared_ptr<TTexture<Image::ArrayType>> genNumericTexture(
		const CookingContext& ctx) = 0;

	virtual std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) = 0;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<Image>)
	{
		ClassType clazz("image");
		clazz.docName("Image");
		clazz.description("A block of data.");
		return clazz;
	}
};

}// end namespace ph
