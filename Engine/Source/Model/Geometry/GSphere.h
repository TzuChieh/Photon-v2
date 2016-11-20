#pragma once

#include "Model/Geometry/Geometry.h"
#include "Common/primitive_type.h"

namespace ph
{

class Model;

class GSphere final : public Geometry
{
public:
	GSphere(const float32 radius);
	GSphere(const GSphere& other);
	virtual ~GSphere() override;

	virtual void genPrimitives(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const Model* const parentModel) const override;

	GSphere& operator = (const GSphere& rhs);

private:
	float32 m_radius;
};

}// end namespace ph