#pragma once

#include "Core/Intersection/Primitive.h"
#include "Math/TVector3.h"
#include "Math/Geometry/TWatertightTriangle.h"

#include <Common/assertion.h>

namespace ph
{

/*! @brief A single triangle.
This triangle type is for constructing simple shapes such as a quad or box. For more complex shapes,
it is better to use a more space efficient primitive type for them.
*/
class PTriangle : public Primitive
{
public:
	PTriangle(const math::Vector3R& vA, const math::Vector3R& vB, const math::Vector3R& vC);

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;

	bool reintersect(
		const Ray& ray,
		HitProbe& probe,
		const Ray& srcRay,
		HitProbe& srcProbe) const override;

	void calcHitDetail(
		const Ray& ray, 
		HitProbe&  probe,
		HitDetail* out_detail) const override;

	bool mayOverlapVolume(const math::AABB3D& volume) const override;
	math::AABB3D calcAABB() const override;

	void genPosSample(
		PrimitivePosSampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	void calcPosPdf(PrimitivePosPdfQuery& query) const override;

	real calcExtendedArea() const override;

	/*! @brief Set vertex normals.
	Vertex normals are set to face normal by default.
	*/
	///@{
	void setNa(const math::Vector3R& nA);
	void setNb(const math::Vector3R& nB);
	void setNc(const math::Vector3R& nC);
	///@}

	/*! @brief Set vertex parametric coordinates.
	Surface parametric coordinates are set to 0 by default.
	*/
	///@{
	void setUVWa(const math::Vector3R& uvwA);
	void setUVWb(const math::Vector3R& uvwB);
	void setUVWc(const math::Vector3R& uvwC);
	///@}

	const math::Vector3R& getNa() const;
	const math::Vector3R& getNb() const;
	const math::Vector3R& getNc() const;

	const math::Vector3R& getUVWa() const;
	const math::Vector3R& getUVWb() const;
	const math::Vector3R& getUVWc() const;

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

inline void PTriangle::setNa(const math::Vector3R& nA)
{
	PH_ASSERT_MSG(nA.isFinite(), nA.toString());
	PH_ASSERT_IN_RANGE(nA.lengthSquared(), 0.9_r, 1.1_r);

	m_nA = nA;
}

inline void PTriangle::setNb(const math::Vector3R& nB)
{
	PH_ASSERT_MSG(nB.isFinite(), nB.toString());
	PH_ASSERT_IN_RANGE(nB.lengthSquared(), 0.9_r, 1.1_r);

	m_nB = nB;
}

inline void PTriangle::setNc(const math::Vector3R& nC)
{
	PH_ASSERT_MSG(nC.isFinite(), nC.toString());
	PH_ASSERT_IN_RANGE(nC.lengthSquared(), 0.9_r, 1.1_r);

	m_nC = nC;
}

inline void PTriangle::setUVWa(const math::Vector3R& uvwA)
{
	m_uvwA = uvwA;
}

inline void PTriangle::setUVWb(const math::Vector3R& uvwB)
{
	m_uvwB = uvwB;
}

inline void PTriangle::setUVWc(const math::Vector3R& uvwC)
{
	m_uvwC = uvwC;
}

inline const math::Vector3R& PTriangle::getNa() const
{
	return m_nC;
}

inline const math::Vector3R& PTriangle::getNb() const
{
	return m_nB;
}

inline const math::Vector3R& PTriangle::getNc() const
{
	return m_nA;
}

inline const math::Vector3R& PTriangle::getUVWa() const
{
	return m_uvwA;
}

inline const math::Vector3R& PTriangle::getUVWb() const
{
	return m_uvwB;
}

inline const math::Vector3R& PTriangle::getUVWc() const
{
	return m_uvwC;
}

}// end namespace ph
