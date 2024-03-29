#include "Actor/LightSource/RectangleSource.h"
#include "Actor/Geometry/GRectangle.h"

#include <Common/assertion.h>

#include <memory>

namespace ph
{
	
RectangleSource::RectangleSource() : 
	RectangleSource(1, 1, math::Vector3R(1, 1, 1), 100)
{}

RectangleSource::RectangleSource(
	const real            width, 
	const real            height, 
	const math::Vector3R& linearSrgbColor,
	const real            numWatts) : 

	AreaSource(linearSrgbColor, numWatts)
{
	setDimension(width, height);
}

RectangleSource::RectangleSource(
	const real             width, 
	const real             height, 
	const math::Spectrum&  color,
	const real             numWatts) : 

	AreaSource(color, numWatts)
{
	setDimension(width, height);
}

std::shared_ptr<Geometry> RectangleSource::genAreas(CookingContext& ctx) const
{
	auto rectangle = TSdl<GRectangle>::makeResource();
	rectangle->setWidth(m_width).setHeight(m_height);

	return rectangle;
}

void RectangleSource::setDimension(const real width, const real height)
{
	PH_ASSERT(width > 0.0_r && height > 0.0_r);

	m_width  = width;
	m_height = height;
}

}// end namespace ph
