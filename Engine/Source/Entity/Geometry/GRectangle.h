#pragma once

#include "Entity/Geometry/Geometry.h"
#include "Common/primitive_type.h"

namespace ph
{

class GRectangle : public Geometry
{
public:
	GRectangle(const float32 width, const float32 height);
	virtual ~GRectangle() override;

	virtual void discretize(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const Entity* const parentEntity) const override;

private:
	float32 m_width;
	float32 m_height;
};

}// end namespace ph