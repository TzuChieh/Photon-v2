#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"

namespace ph
{

class GTriangle : public Geometry
{
public:
	GTriangle(const math::Vector3R& vA, const math::Vector3R& vB, const math::Vector3R& vC);

	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const override;

	bool isDegenerate() const;

	inline const math::Vector3R getVa() const
	{
		return m_vA;
	}

	inline const math::Vector3R getVb() const
	{
		return m_vB;
	}

	inline const math::Vector3R getVc() const
	{
		return m_vC;
	}

	inline const math::Vector3R getNa() const
	{
		return m_nA;
	}

	inline const math::Vector3R getNb() const
	{
		return m_nB;
	}

	inline const math::Vector3R getNc() const
	{
		return m_nC;
	}

	inline const math::Vector3R getUVWa() const
	{
		return m_uvwA;
	}

	inline const math::Vector3R getUVWb() const
	{
		return m_uvwB;
	}

	inline const math::Vector3R getUVWc() const
	{
		return m_uvwC;
	}

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

private:
	math::Vector3R m_vA;
	math::Vector3R m_vB;
	math::Vector3R m_vC;

	math::Vector3R m_nA;
	math::Vector3R m_nB;
	math::Vector3R m_nC;

	math::Vector3R m_uvwA;
	math::Vector3R m_uvwB;
	math::Vector3R m_uvwC;
};

}// end namespace ph
