#pragma once

#include "Engine/Actor/Image/Image.h"
#include "Engine/Math/math_fwd.h"
#include "Engine/SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class LuminanceImage : public Image
{
public:
	std::shared_ptr<TTexture<Image::NumericType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<real>> genRealTexture(const CookingContext& ctx) override;

	LuminanceImage& setInput(std::shared_ptr<Image> image);
	const std::shared_ptr<Image>& getInput() const;

private:
	std::shared_ptr<Image> m_input;

public:
	PH_DEFINE_SDL_CLASS(LuminanceImage, clazz)
	{
		clazz.typeName("luminance");
		clazz.docName("Luminance Image");
		clazz.description("Converts a source image to color-managed relative luminance.");
		clazz.baseOn<Image>();

		TSdlReference<Image, OwnerType> input("input", &OwnerType::m_input);
		input.description("Reference to the source image.");
		clazz.addField(input);
	}
};

}// end namespace ph
