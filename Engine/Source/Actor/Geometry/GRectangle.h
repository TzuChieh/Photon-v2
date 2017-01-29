#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"

namespace ph
{

class GRectangle : public Geometry
{
public:
	GRectangle(const float32 width, const float32 height);
	GRectangle(const InputPacket& packet);
	virtual ~GRectangle() override;

	virtual void discretize(const PrimitiveBuildingMaterial& data,
	                        std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

private:
	float32 m_width;
	float32 m_height;

	static bool checkData(const PrimitiveBuildingMaterial& data, const float32 width, const float32 height);
};

}// end namespace ph