#pragma once

#include "Model/Geometry/Geometry.h"
#include "Model/Geometry/Primitive.h"
#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

namespace ph
{

class Sphere : public Geometry, public Primitive
{
public:
	Sphere(const Vector3f& center, const float32 radius);
	Sphere(const Sphere& other);
	virtual ~Sphere() override;

	virtual void genPrimitives(std::vector<std::unique_ptr<Primitive>>* const out_primitives) const override;
	virtual bool isIntersecting(const Ray& ray, Intersection* const out_intersection) const override;

	Sphere& operator = (const Sphere& rhs);

private:
	Vector3f m_center;
	float32  m_radius;
};

}// end namespace ph