#pragma once

#include "Engine/Actor/Actor.h"
#include "Engine/Actor/Image/Image.h"
#include "Engine/SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class AMaskedModel : public Actor
{
public:
	PreCookReport preCook(const CookingContext& ctx) const override;
	TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) const override;

	void setMask(const std::shared_ptr<Image>& mask);

private:
	std::shared_ptr<Actor> m_base;
	std::shared_ptr<Image> m_mask;

public:
	PH_DEFINE_SDL_CLASS(AMaskedModel, clazz)
	{
		clazz.typeName("masked-model");
		clazz.docName("Masked Model Actor");
		clazz.description(
			"Attempts to apply an additional actor-wide mask to any non-emitting actor. For "
			"per-material masking, use the material's interface-mask field.");
		clazz.baseOn<Actor>();

		TSdlReference<Actor, OwnerType> base("base", &OwnerType::m_base);
		base.description(
			"The base actor that the mask will apply on. Declare it as phantom to hide the "
			"unmasked base.");
		base.required();
		clazz.addField(base);

		TSdlReference<Image, OwnerType> mask("mask", &OwnerType::m_mask);
		mask.description(
			"The mask to apply. This will remove some part of the base model acoording to the "
			"pattern recorded on the image.");
		mask.required();
		clazz.addField(mask);
	}
};

}// end namespace ph
