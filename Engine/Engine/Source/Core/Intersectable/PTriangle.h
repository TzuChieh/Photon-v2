#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Math/TVector3.h"
#include "Math/Geometry/TWatertightTriangle.h"

#include <Common/assertion.h>

namespace ph
{

class Ray;

class PTriangle : public Primitive
{
public:
	PTriangle(const math::Vector3R& vA, const math::Vector3R& vB, const math::Vector3R& vC);

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	void calcIntersectionDetail(const Ray& ray, HitProbe& probe,
	                            HitDetail* out_detail) const override;
	bool mayOverlapVolume(const math::AABB3D& volume) const override;
	math::AABB3D calcAABB() const override;
	real calcPositionSamplePdfA(const math::Vector3R& position) const override;
	void genPositionSample(PrimitivePosSampleQuery& query, SampleFlow& sampleFlow) const override;

	real calcExtendedArea() const override;

	// TODO: update internal data like area when setters are called

	inline void setNa(const math::Vector3R& nA)
	{
		PH_ASSERT_MSG(nA.isFinite() && nA.length() > 0.9_r, nA.toString());

		m_nA = nA;
	}

	inline void setNb(const math::Vector3R& nB)
	{
		PH_ASSERT_MSG(nB.isFinite() && nB.length() > 0.9_r, nB.toString());

		m_nB = nB;
	}

	inline void setNc(const math::Vector3R& nC)
	{
		PH_ASSERT_MSG(nC.isFinite() && nC.length() > 0.9_r, nC.toString());

		m_nC = nC;
	}

	inline void setUVWa(const math::Vector3R& uvwA)
	{
		m_uvwA = uvwA;
	}

	inline void setUVWb(const math::Vector3R& uvwB)
	{
		m_uvwB = uvwB;
	}

	inline void setUVWc(const math::Vector3R& uvwC)
	{
		m_uvwC = uvwC;
	}

	inline const math::Vector3R& getUVWa() const
	{
		return m_uvwA;
	}

	inline const math::Vector3R& getUVWb() const
	{
		return m_uvwB;
	}

	inline const math::Vector3R& getUVWc() const
	{
		return m_uvwC;
	}

private:
	using Triangle = math::TWatertightTriangle<real>;

	Triangle m_triangle;

	math::Vector3R m_nA;
	math::Vector3R m_nB;
	math::Vector3R m_nC;

	math::Vector3R m_uvwA;
	math::Vector3R m_uvwB;
	math::Vector3R m_uvwC;

	math::Vector3R m_faceNormal;
};

}// end namespace ph
