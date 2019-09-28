#pragma once

#include "Math/TVector3.h"
#include "Core/Intersectable/Primitive.h"
#include "Common/assertion.h"

namespace ph
{

class Ray;

class PTriangle final : public Primitive
{
public:
	PTriangle(const PrimitiveMetadata* metadata, const Vector3R& vA, const Vector3R& vB, const Vector3R& vC);

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	void calcIntersectionDetail(const Ray& ray, HitProbe& probe,
	                            HitDetail* out_detail) const override;
	bool isIntersectingVolumeConservative(const AABB3D& volume) const override;
	void calcAABB(AABB3D* out_aabb) const override;
	real calcPositionSamplePdfA(const Vector3R& position) const override;
	void genPositionSample(PositionSample* out_sample) const override;

	real calcExtendedArea() const override;

	// TODO: update internal data like area when setters are called

	inline void setNa(const Vector3R& nA)
	{
		PH_ASSERT_MSG(nA.isFinite() && nA.length() > 0.9_r, nA.toString());

		m_nA = nA;
	}

	inline void setNb(const Vector3R& nB)
	{
		PH_ASSERT_MSG(nB.isFinite() && nB.length() > 0.9_r, nB.toString());

		m_nB = nB;
	}

	inline void setNc(const Vector3R& nC)
	{
		PH_ASSERT_MSG(nC.isFinite() && nC.length() > 0.9_r, nC.toString());

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

	Vector3R m_faceNormal;

	Vector3R calcBarycentricCoord(const Vector3R& position) const;
};

}// end namespace ph
