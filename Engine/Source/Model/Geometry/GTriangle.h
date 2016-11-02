#pragma once

#include "Model/Geometry/Geometry.h"
#include "Math/Vector3f.h"

namespace ph
{

class GTriangle : public Geometry
{
public:
	GTriangle(const Vector3f& vA, const Vector3f& vB, const Vector3f& vC);
	virtual ~GTriangle() override;

	virtual void genPrimitives(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const Model* const parentModel) const override;

private:
	Vector3f m_vA;
	Vector3f m_vB;
	Vector3f m_vC;
};

}// end namespace ph