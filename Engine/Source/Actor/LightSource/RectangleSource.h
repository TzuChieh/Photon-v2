#pragma once

#include "Actor/LightSource/AreaSource.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class RectangleSource final : public AreaSource
{
public:
	RectangleSource();
	RectangleSource(real width, real height, const math::Vector3R& linearSrgbColor, real numWatts);
	RectangleSource(real width, real height, const math::Spectrum& color, real numWatts);

	std::shared_ptr<Geometry> genAreas(ActorCookingContext& ctx) const override;

	void setDimension(real width, real height);

private:
	real m_width;
	real m_height;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<RectangleSource>)
	{
		ClassType clazz("rectangle");
		clazz.docName("Rectangular Light Source");
		clazz.description(
			"This type of light emits energy from a rectangular shape.Note that energy"
			"is only allowed to emit from one side of the rectangle, not both sides.");
		clazz.baseOn<AreaSource>();

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
