#pragma once

#include "SDL/TSdlResourceBase.h"
#include "Math/TVector3.h"
#include "Core/Texture/TTexture.h"
#include "Math/Color/Spectrum.h"
#include "SDL/sdl_interface.h"
#include "Math/TArithmeticArray.h"

#include <Common/config.h>
#include <Common/primitive_type.h>

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
	/*!
	When generating numeric textures, the convention is that the image will be treated as if
	raw values are used. Numeric textures are expected to be used as part of a general
	mathematical operation.
	*/
	virtual std::shared_ptr<TTexture<Image::ArrayType>> genNumericTexture(
		const CookingContext& ctx) = 0;

	/*!
	Color textures are expected to be used where spectral distribution is involved. Color space
	transformations will take place if required. It is generally not recommended to use color
	texture to represent data that need to be precisely preserved (e.g., normal map).
	*/
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
