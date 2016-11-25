#pragma once

#include "Math/Vector3f.h"

namespace ph
{

class Model;
class Ray;
class Intersection;
class AABB;

class Triangle final
{
public:
	Triangle(const Model* const parentModel, const Vector3f& vA, const Vector3f& vB, const Vector3f& vC);
	~Triangle() = default;

	bool isIntersecting(const Ray& ray, Intersection* const out_intersection) const;
	void calcAABB(AABB* const out_aabb) const;

	inline const Model* getParentModel() const
	{
		return m_parentModel;
	}

private:
	const Model* m_parentModel;

	Vector3f m_vA;
	Vector3f m_vB;
	Vector3f m_vC;

	Vector3f m_eAB;
	Vector3f m_eAC;

	Vector3f m_faceNormal;
};

}// end namespace ph