#pragma once

#include "Model/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

namespace ph
{

class Sphere : public Geometry
{
public:
	Sphere(const Vector3f& center, const float32 radius);

	virtual bool isIntersecting(const Ray& ray, Intersection* const out_intersection) const override;

private:
	Vector3f m_center;
	float32  m_radius;
};

}// end namespace ph