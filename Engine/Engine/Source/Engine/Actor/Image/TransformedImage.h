#pragma once

#include "Engine/Actor/Basic/TransformInfo.h"
#include "Engine/Actor/Image/Image.h"
#include "Engine/SDL/sdl_interface.h"

#include <memory>

namespace ph
{

/*! @brief Samples an image with affine-transformed UVW coordinates.
*/
class TransformedImage : public Image
{
public:
	std::shared_ptr<TTexture<Image::NumericType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	TransformedImage& setInput(std::shared_ptr<Image> input);
	TransformedImage& setTransform(const TransformInfo& transform);

	Image* getInput() const;
	const TransformInfo& getTransform() const;

private:
	std::shared_ptr<Image> m_input;
	TransformInfo m_transform;

public:
	PH_DEFINE_SDL_CLASS(TransformedImage, clazz)
	{
		clazz.typeName("transformed");
		clazz.docName("Transformed Image");
		clazz.description(
			"An image sampled with affine-transformed UVW coordinates. Scale is applied first, "
			"followed by rotation and translation.");
		clazz.baseOn<Image>();

		TSdlReference<Image, OwnerType> input("input", &OwnerType::m_input);
		input.description("The image to sample with transformed coordinates.");
		input.required();
		clazz.addField(input);

		clazz.addStruct(&OwnerType::m_transform);
	}
};

}// end namespace ph
