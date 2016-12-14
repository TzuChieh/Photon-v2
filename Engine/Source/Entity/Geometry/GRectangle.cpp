#include "Entity/Geometry/GRectangle.h"
#include "Entity/Geometry/Triangle.h"
#include "Math/Vector3f.h"

#include <iostream>

namespace ph
{

GRectangle::GRectangle(const float32 width, const float32 height) :
	m_width(width), m_height(height)
{

}

GRectangle::~GRectangle() = default;

void GRectangle::discretize(std::vector<Triangle>* const out_triangles, const Entity* const parentEntity) const
{
	if(m_width <= 0.0f || m_height <= 0.0f)
	{
		std::cerr << "warning: GRectangle's dimension is zero or negative" << std::endl;
	}

	const float32 halfWidth = m_width * 0.5f;
	const float32 halfHeight = m_height * 0.5f;

	const Vector3f vA(-halfWidth,  halfHeight, 0.0f);
	const Vector3f vB(-halfWidth, -halfHeight, 0.0f);
	const Vector3f vC( halfWidth, -halfHeight, 0.0f);
	const Vector3f vD( halfWidth,  halfHeight, 0.0f);

	// 2 triangles for a rectangle (both CCW)
	out_triangles->push_back(Triangle(parentEntity, vA, vB, vD));
	out_triangles->push_back(Triangle(parentEntity, vB, vC, vD));
}

}// end namespace ph