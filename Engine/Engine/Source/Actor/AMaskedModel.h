#pragma once

#include "Actor/Actor.h"
#include "Actor/AModel.h"
#include "Actor/Image/Image.h"
#include "SDL/sdl_interface.h"

#include <memory>
#include <vector>

namespace ph
{

class AMaskedModel : public Actor
{
public:
	PreCookReport preCook(const CookingContext& ctx) const override;
	TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) const override;

	void setMask(const std::shared_ptr<Image>& mask);

private:
	std::shared_ptr<AModel> m_base;
	std::shared_ptr<Image> m_mask;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<AMaskedModel>)
	{
		ClassType clazz("masked-model");
		clazz.docName("Masked Model Actor");
		clazz.description("Masks another model actor.");
		clazz.baseOn<Actor>();

		TSdlReference<AModel, OwnerType> base("base", &OwnerType::m_base);
		base.description(
			"The base model that the mask will apply on.");
		base.required();
		clazz.addField(base);

		TSdlReference<Image, OwnerType> mask("mask", &OwnerType::m_mask);
		mask.description(
			"The mask to apply. This will remove some part of the base model acoording to the "
			"pattern recorded on the image.");
		mask.required();
		clazz.addField(mask);

		return clazz;
	}
};

}// end namespace ph
