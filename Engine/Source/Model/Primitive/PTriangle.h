#pragma once

#include "Model/Primitive/Primitive.h"
#include "Math/Vector3f.h"

namespace ph
{

class GTriangle;

class PTriangle : public Primitive
{
public:
	PTriangle(const GTriangle* gTriangle, const Model* const parentModel);
	virtual ~PTriangle() override;

	virtual bool isIntersecting(const Ray& ray, Intersection* const out_intersection) const override;
	virtual void calcAABB(AABB* const out_aabb) const override;

private:
	const GTriangle* m_gTriangle;

	Vector3f m_eAB;
	Vector3f m_eAC;

	Vector3f m_faceNormal;
};

}// end namespace ph