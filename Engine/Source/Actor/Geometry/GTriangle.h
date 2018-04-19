#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"

namespace ph
{

class GTriangle final : public Geometry
{
public:
	GTriangle(const Vector3R& vA, const Vector3R& vB, const Vector3R& vC);
	//GTriangle(const InputPacket& packet);
	virtual ~GTriangle() override;

	virtual void genPrimitive(const PrimitiveBuildingMaterial& data,
	                          std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;
	virtual std::shared_ptr<Geometry> genTransformApplied(const StaticTransform& transform) const override;

	bool isDegenerate() const;

	inline const Vector3R getVa() const
	{
		return m_vA;
	}

	inline const Vector3R getVb() const
	{
		return m_vB;
	}

	inline const Vector3R getVc() const
	{
		return m_vC;
	}

	inline const Vector3R getNa() const
	{
		return m_nA;
	}

	inline const Vector3R getNb() const
	{
		return m_nB;
	}

	inline const Vector3R getNc() const
	{
		return m_nC;
	}

	inline const Vector3R getUVWa() const
	{
		return m_uvwA;
	}

	inline const Vector3R getUVWb() const
	{
		return m_uvwB;
	}

	inline const Vector3R getUVWc() const
	{
		return m_uvwC;
	}

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
};

}// end namespace ph