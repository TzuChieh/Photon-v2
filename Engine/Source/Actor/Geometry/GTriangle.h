#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Math/Vector3f.h"

namespace ph
{

class GTriangle final : public Geometry
{
public:
	GTriangle(const Vector3f& vA, const Vector3f& vB, const Vector3f& vC);
	virtual ~GTriangle() override;

	virtual void discretize(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const PrimitiveMetadata& metadata) const override;

	inline const Vector3f getVa() const
	{
		return m_vA;
	}

	inline const Vector3f getVb() const
	{
		return m_vB;
	}

	inline const Vector3f getVc() const
	{
		return m_vC;
	}

	inline const Vector3f getNa() const
	{
		return m_nA;
	}

	inline const Vector3f getNb() const
	{
		return m_nB;
	}

	inline const Vector3f getNc() const
	{
		return m_nC;
	}

	inline const Vector3f getUVWa() const
	{
		return m_uvwA;
	}

	inline const Vector3f getUVWb() const
	{
		return m_uvwB;
	}

	inline const Vector3f getUVWc() const
	{
		return m_uvwC;
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
};

}// end namespace ph