#pragma once

#include "Actor/Geometry/GTriangle.h"

#include <vector>
#include <memory>

namespace ph
{

class GTriangleMesh final : public Geometry
{
public:
	GTriangleMesh();
	GTriangleMesh(const InputPacket& packet);
	virtual ~GTriangleMesh() override;

	virtual void discretize(const PrimitiveBuildingMaterial& data,
	                        std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	void addTriangle(const GTriangle gTriangle);

private:
	std::vector<GTriangle> m_gTriangles;
};

}// end namespace ph