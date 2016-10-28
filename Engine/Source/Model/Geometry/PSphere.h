#pragma once

#include "Model/Geometry/Primitive.h"
#include "Math/Vector3f.h"

namespace ph
{


class PSphere : public Primitive
{
public:
	PSphere(const Vector3f& center, const float32 radius, const Model* const parentModel);
	PSphere(const PSphere& other);
	virtual ~PSphere() override;

	virtual bool isIntersecting(const Ray& ray, Intersection* const out_intersection) const override;

	PSphere& operator = (const PSphere& rhs);

private:
	Vector3f m_center;
	float32 m_radius;
};

}// end namespace ph