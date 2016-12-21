#pragma once

#include "Entity/Geometry/GTriangle.h"

#include <vector>
#include <memory>

namespace ph
{

class GTriangleMesh final : public Geometry
{
public:
	virtual ~GTriangleMesh() override;

	virtual void discretize(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const PrimitiveMetadata* const metadata) const override;

	void addTriangle(const GTriangle gTriangle);

private:
	std::vector<GTriangle> m_gTriangles;
};

}// end namespace ph