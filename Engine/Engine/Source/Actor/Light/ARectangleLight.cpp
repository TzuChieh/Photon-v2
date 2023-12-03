#include "Actor/Light/ARectangleLight.h"
#include "Common/assertion.h"
#include "Actor/Geometry/GRectangle.h"

namespace ph
{

std::shared_ptr<Geometry> ARectangleLight::getArea(const CookingContext& ctx) const
{
	auto rectangle = TSdl<GRectangle>::makeResource();
	rectangle->setWidth(m_width).setHeight(m_height);
	return rectangle;
}

void ARectangleLight::setDimension(const real width, const real height)
{
	PH_ASSERT_GT(width, 0.0_r);
	PH_ASSERT_GT(height, 0.0_r);

	m_width = width;
	m_height = height;
}

}// end namespace ph
