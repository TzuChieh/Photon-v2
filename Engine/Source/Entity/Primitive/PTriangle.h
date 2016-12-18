#pragma once

#include "Math/Vector3f.h"
#include "Entity/Primitive/Primitive.h"

namespace ph
{

class Entity;
class Ray;
class Intersection;
class AABB;

class PTriangle final : public Primitive
{
public:
	PTriangle(const Entity* const parentEntity, const Vector3f& vA, const Vector3f& vB, const Vector3f& vC);
	virtual ~PTriangle() override;

	virtual bool isIntersecting(const Ray& ray, Intersection* const out_intersection) const override;
	virtual bool isIntersecting(const AABB& aabb) const override;
	virtual void calcAABB(AABB* const out_aabb) const override;

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