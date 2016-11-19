#pragma once

#include "Model/Geometry/Geometry.h"
#include "Math/Vector3f.h"

namespace ph
{

class GTriangle final : public Geometry
{
public:
	GTriangle(const Vector3f& vA, const Vector3f& vB, const Vector3f& vC);
	virtual ~GTriangle() override;

	virtual void genPrimitives(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const Model* const parentModel) const override;

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

private:
	Vector3f m_vA;
	Vector3f m_vB;
	Vector3f m_vC;
};

}// end namespace ph