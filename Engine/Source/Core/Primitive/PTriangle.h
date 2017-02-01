#pragma once

#include "Math/TVector3.h"
#include "Core/Primitive/Primitive.h"

namespace ph
{

class Ray;
class Intersection;
class AABB;

class PTriangle final : public Primitive
{
public:
	PTriangle(const PrimitiveMetadata* const metadata, const Vector3R& vA, const Vector3R& vB, const Vector3R& vC);
	virtual ~PTriangle() override;

	virtual bool isIntersecting(const Ray& ray, Intersection* const out_intersection) const override;
	virtual bool isIntersecting(const Ray& ray) const override;
	virtual bool isIntersectingVolume(const AABB& aabb) const override;
	virtual void calcAABB(AABB* const out_aabb) const override;
	virtual real calcPositionSamplePdfA(const Vector3R& position) const override;
	virtual void genPositionSample(PositionSample* const out_sample) const override;

	virtual real calcExtendedArea() const override;

	// TODO: update internal data like area when setters are called

	inline void setNa(const Vector3R& nA)
	{
		m_nA = nA;
	}

	inline void setNb(const Vector3R& nB)
	{
		m_nB = nB;
	}

	inline void setNc(const Vector3R& nC)
	{
		m_nC = nC;
	}

	inline void setUVWa(const Vector3R& uvwA)
	{
		m_uvwA = uvwA;
	}

	inline void setUVWb(const Vector3R& uvwB)
	{
		m_uvwB = uvwB;
	}

	inline void setUVWc(const Vector3R& uvwC)
	{
		m_uvwC = uvwC;
	}

	inline const Vector3R& getUVWa() const
	{
		return m_uvwA;
	}

	inline const Vector3R& getUVWb() const
	{
		return m_uvwB;
	}

	inline const Vector3R& getUVWc() const
	{
		return m_uvwC;
	}

private:
	Vector3R m_vA;
	Vector3R m_vB;
	Vector3R m_vC;

	Vector3R m_nA;
	Vector3R m_nB;
	Vector3R m_nC;

	Vector3R m_uvwA;
	Vector3R m_uvwB;
	Vector3R m_uvwC;

	Vector3R m_eAB;
	Vector3R m_eAC;

	Vector3R m_faceNormal;

	Vector3R calcBarycentricCoord(const Vector3R& position) const;
};

}// end namespace ph