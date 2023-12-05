#pragma once

#include "Actor/Light/AAreaLight.h"
#include "SDL/sdl_interface.h"

#include <Common/primitive_type.h>

namespace ph
{

class ARectangleLight : public AAreaLight
{
public:
	std::shared_ptr<Geometry> getArea(const CookingContext& ctx) const override;

	void setDimension(real width, real height);

private:
	real m_width;
	real m_height;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<ARectangleLight>)
	{
		ClassType clazz("rectangle-light");
		clazz.docName("Rectangular Light Actor");
		clazz.description(
			"This type of light emits energy from a rectangular shape.Note that energy"
			"is only allowed to emit from one side of the rectangle, not both sides.");
		clazz.baseOn<AAreaLight>();

		TSdlReal<OwnerType> width("width", &OwnerType::m_width);
		width.description("The width of the rectangle.");
		width.defaultTo(1);
		clazz.addField(width);

		TSdlReal<OwnerType> height("height", &OwnerType::m_height);
		height.description("The height of the rectangle.");
		height.defaultTo(1);
		clazz.addField(height);

		return clazz;
	}
};

}// end namespace ph
