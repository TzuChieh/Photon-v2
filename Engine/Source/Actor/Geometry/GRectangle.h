#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"

namespace ph
{

class GRectangle : public Geometry
{
public:
	GRectangle(const real width, const real height);
	GRectangle(const InputPacket& packet);
	virtual ~GRectangle() override;

	virtual void discretize(const PrimitiveBuildingMaterial& data,
	                        std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

private:
	real m_width;
	real m_height;

	static bool checkData(const PrimitiveBuildingMaterial& data, const real width, const real height);
};

}// end namespace ph