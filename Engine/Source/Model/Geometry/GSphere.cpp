#include "Model/Geometry/GSphere.h"
#include "Model/Primitive/PSphere.h"

#include <cmath>

namespace ph
{

GSphere::GSphere(const Vector3f& center, const float32 radius) :
	m_center(center), m_radius(radius)
{

}

GSphere::GSphere(const GSphere& other) :
	m_center(other.m_center), m_radius(other.m_radius)
{

}

GSphere::~GSphere() = default;

void GSphere::genPrimitives(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const Model* const parentModel) const
{
	out_primitives->push_back(std::make_unique<PSphere>(m_center, m_radius, parentModel));
}

GSphere& GSphere::operator = (const GSphere& rhs)
{
	m_center.set(rhs.m_center);
	m_radius = rhs.m_radius;

	return *this;
}

}// end namespace ph