#pragma once

#include "Model/Geometry/Geometry.h"
#include "Common/primitive_type.h"

namespace ph
{

class GRectangle : public Geometry
{
public:
	GRectangle(const float32 width, const float32 height);
	virtual ~GRectangle() override;

	virtual void discretize(std::vector<Triangle>* const out_triangles, const Model* const parentModel) const override;

private:
	float32 m_width;
	float32 m_height;
};

}// end namespace ph