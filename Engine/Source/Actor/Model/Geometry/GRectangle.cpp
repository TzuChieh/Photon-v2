#include "Actor/Model/Geometry/GRectangle.h"
#include "Core/Primitive/PTriangle.h"
#include "Math/Vector3f.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Actor/Model/Model.h"
#include "Core/Primitive/PrimitiveStorage.h"

#include <iostream>

namespace ph
{

GRectangle::GRectangle(const float32 width, const float32 height) :
	m_width(width), m_height(height)
{

}

GRectangle::~GRectangle() = default;

void GRectangle::discretize(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const PrimitiveMetadata& metadata) const
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
	out_primitives->push_back(std::make_unique<PTriangle>(&metadata, vA, vB, vD));
	out_primitives->push_back(std::make_unique<PTriangle>(&metadata, vB, vC, vD));
}

}// end namespace ph