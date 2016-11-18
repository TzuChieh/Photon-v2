#pragma once

#include "Model/Primitive/Primitive.h"
#include "Math/Vector3f.h"

namespace ph
{

class PTriangle : public Primitive
{
public:
	PTriangle(const Vector3f& vA, const Vector3f& vB, const Vector3f& vC, const Model* const parentModel);
	virtual ~PTriangle() override;

	virtual bool isIntersecting(const Ray& ray, Intersection* const out_intersection) const override;
	virtual void calcAABB(AABB* const out_aabb) const override;

private:
	Vector3f m_vA;
	Vector3f m_vB;
	Vector3f m_vC;

	Vector3f m_eAB;
	Vector3f m_eAC;

	Vector3f m_faceNormal;
};

}// end namespace ph