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

	void genPrimitive(
		const PrimitiveBuildingMaterial&         data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const override;

	void add(const std::shared_ptr<Geometry>& geometry);

	bool addTransformed(
		const std::shared_ptr<Geometry>& geometry, 
		const math::StaticAffineTransform& transform);

private:
	std::vector<std::shared_ptr<Geometry>> m_geometries;
};

}// end namespace ph
