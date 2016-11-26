#pragma once

#include "Math/Vector3f.h"

namespace ph
{

class Model;
class Ray;
class Intersection;
class AABB;
class DifferentialPatch;

class Triangle final
{
public:
	Triangle(const Model* const parentModel, const Vector3f& vA, const Vector3f& vB, const Vector3f& vC);
	~Triangle() = default;

	bool isIntersecting(const Ray& ray, Intersection* const out_intersection) const;
	void calcAABB(AABB* const out_aabb) const;
	void calcDifferentialPatch(const Vector3f& hitPosition, DifferentialPatch* const out_differentialPatch) const;

	inline const Model* getParentModel() const
	{
		return m_parentModel;
	}

	inline void setNa(const Vector3f& nA)
	{
		m_nA = nA;
	}

	inline void setNb(const Vector3f& nB)
	{
		m_nB = nB;
	}

	inline void setNc(const Vector3f& nC)
	{
		m_nC = nC;
	}

	inline void setUVWa(const Vector3f& uvwA)
	{
		m_uvwA = uvwA;
	}

	inline void setUVWb(const Vector3f& uvwB)
	{
		m_uvwB = uvwB;
	}

	inline void setUVWc(const Vector3f& uvwC)
	{
		m_uvwC = uvwC;
	}

	inline const Vector3f& getUVWa() const
	{
		return m_uvwA;
	}

	inline const Vector3f& getUVWb() const
	{
		return m_uvwB;
	}

	inline const Vector3f& getUVWc() const
	{
		return m_uvwC;
	}

private:
	const Model* m_parentModel;

	Vector3f m_vA;
	Vector3f m_vB;
	Vector3f m_vC;

	Vector3f m_nA;
	Vector3f m_nB;
	Vector3f m_nC;

	Vector3f m_uvwA;
	Vector3f m_uvwB;
	Vector3f m_uvwC;

	Vector3f m_eAB;
	Vector3f m_eAC;

	Vector3f m_faceNormal;
};

}// end namespace ph