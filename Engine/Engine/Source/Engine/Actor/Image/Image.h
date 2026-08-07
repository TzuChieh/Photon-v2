#pragma once

#include "Engine/SDL/TSdlResourceBase.h"
#include "Engine/Math/math_fwd.h"
#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/SDL/sdl_interface.h"
#include "Engine/Math/TArithmeticArray.h"

#include <Common/config.h>
#include <Common/primitive_type.h>

#include <memory>

namespace ph { class CookingContext; }

namespace ph
{

class Image : public TSdlResourceBase<ESdlTypeCategory::Ref_Image>
{
public:
	using NumericElementType = float64;
	using NumericType = math::TArithmeticArray<NumericElementType, PH_NUMERIC_IMAGE_MAX_ELEMENTS>;
	
	inline static constexpr auto NUMERIC_TYPE_WIDTH = NumericType::NUM_ELEMENTS;

public:
	/*!
	When generating numeric textures, the convention is that the image will be treated as if
	raw values are used. Numeric textures are expected to be used as part of a general
	mathematical operation.
	*/
	virtual std::shared_ptr<TTexture<Image::NumericType>> genNumericTexture(
		const CookingContext& ctx) = 0;

	/*!
	Color textures are expected to be used where spectral distribution is involved. Color space
	transformations will take place if required. It is generally not recommended to use color
	texture to represent data that need to be precisely preserved (e.g., normal map).
	*/
	virtual std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) = 0;

	/*! @brief Generates numeric texture of an alternative type.
	Effectively converting the numeric texture to a different type. Truncates `NumericType` if the
	resulting type is narrower, and appends 0 if the resulting type is wider.
	*/
	///@{
	virtual std::shared_ptr<TTexture<real>> genRealTexture(const CookingContext& ctx);
	virtual std::shared_ptr<TTexture<math::Vector2R>> genVector2RTexture(const CookingContext& ctx);
	virtual std::shared_ptr<TTexture<math::Vector3R>> genVector3RTexture(const CookingContext& ctx);
	virtual std::shared_ptr<TTexture<math::Vector4R>> genVector4RTexture(const CookingContext& ctx);
	///@}

public:
	PH_DEFINE_SDL_CLASS(Image, clazz)
	{
		clazz.typeName("image");
		clazz.docName("Image");
		clazz.description("A block of data.");
		clazz.addField(makeDisplayNameField<OwnerType>());
	}
};

}// end namespace ph
