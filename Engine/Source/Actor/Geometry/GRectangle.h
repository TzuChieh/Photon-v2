#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"

namespace ph
{

class GTriangleMesh;

class GRectangle : public Geometry
{
public:
	GRectangle();
	GRectangle(real width, real height);

	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const override;

	void setTexCoordScale(const real scale);

private:
	real m_width;
	real m_height;
	real m_texCoordScale;

	std::shared_ptr<GTriangleMesh> genTriangleMesh() const;

	static bool checkData(const PrimitiveBuildingMaterial& data, const real width, const real height);
};

}// end namespace ph
