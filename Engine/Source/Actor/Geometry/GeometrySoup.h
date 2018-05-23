#pragma once

#include "Actor/Geometry/Geometry.h"

#include <vector>
#include <memory>

namespace ph
{

class GeometrySoup : public Geometry
{
public:
	GeometrySoup();
	virtual ~GeometrySoup() override;

	virtual void genPrimitive(const PrimitiveBuildingMaterial& data,
	                          std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;
	virtual std::shared_ptr<Geometry> genTransformApplied(const StaticAffineTransform& transform) const override;

	void addGeometry(const std::shared_ptr<Geometry>& geometry);

private:
	std::vector<std::shared_ptr<Geometry>> m_geometries;
};

}// end namespace ph