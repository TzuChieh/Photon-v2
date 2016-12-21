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

	virtual void discretize(PrimitiveStorage* const out_data, const Entity& parentEntity) const override;

private:
	float32 m_width;
	float32 m_height;
};

}// end namespace ph