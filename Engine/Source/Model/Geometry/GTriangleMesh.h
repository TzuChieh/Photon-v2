#pragma once

#include "Model/Geometry/Geometry.h"
#include "Model/Geometry/GTriangle.h"

#include <vector>
#include <memory>

namespace ph
{

class GTriangle;

class GTriangleMesh final : public Geometry
{
public:
	virtual ~GTriangleMesh() override;

	virtual void genPrimitives(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const Model* const parentModel) const override;

	void addTriangle(const std::shared_ptr<GTriangle>& triangle);

private:
	std::vector<std::shared_ptr<GTriangle>> m_gTriangles;
};

}// end namespace ph