#include "Model/Geometry/GSphere.h"
#include "Model/Primitive/PSphere.h"
#include "Math/Vector3f.h"
#include "Model/Model.h"

#include <cmath>
#include <iostream>

namespace ph
{

GSphere::GSphere(const float32 radius) :
	m_radius(radius)
{

}

GSphere::GSphere(const GSphere& other) :
	m_radius(other.m_radius)
{

}

GSphere::~GSphere() = default;

void GSphere::genPrimitives(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const Model* const parentModel) const
{
	Vector3f center;
	float32 radius;
	parentModel->getModelToWorldTransform()->transformPoint(Vector3f(0, 0, 0), &center);
	radius = m_radius * parentModel->getScale().x;

	if(!(parentModel->getScale().x == parentModel->getScale().y && parentModel->getScale().y == parentModel->getScale().z))
	{
		std::cerr << "warning: nonuniform scale on GSphere detected" << std::endl;
	}

	out_primitives->push_back(std::make_unique<PSphere>(center, radius, parentModel));
}

GSphere& GSphere::operator = (const GSphere& rhs)
{
	m_radius = rhs.m_radius;

	return *this;
}

}// end namespace ph