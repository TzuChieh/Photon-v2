#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"

#include <vector>

namespace ph
{

class GTriangleMesh;

class GSphere : public Geometry
{
public:
	GSphere();
	explicit GSphere(real radius);
	GSphere(const GSphere& other);

	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const override;

	GSphere& operator = (const GSphere& rhs);

private:
	real m_radius;

	std::size_t addVertex(const math::Vector3R& vertex, std::vector<math::Vector3R>* const out_vertices) const;
	std::size_t addMidpointVertex(const std::size_t iA, const std::size_t iB, std::vector<math::Vector3R>* const out_vertices) const;
	std::shared_ptr<GTriangleMesh> genTriangleMesh() const;
};

}// end namespace ph
