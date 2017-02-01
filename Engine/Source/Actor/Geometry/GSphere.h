#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"

#include <vector>

namespace ph
{

class GSphere final : public Geometry
{
public:
	GSphere(const real radius);
	GSphere(const GSphere& other);
	GSphere(const InputPacket& packet);
	virtual ~GSphere() override;

	virtual void discretize(const PrimitiveBuildingMaterial& data,
	                        std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	GSphere& operator = (const GSphere& rhs);

private:
	real m_radius;

	std::size_t addVertex(const Vector3R& vertex, std::vector<Vector3R>* const out_vertices) const;
	std::size_t addMidpointVertex(const std::size_t iA, const std::size_t iB, std::vector<Vector3R>* const out_vertices) const;
};

}// end namespace ph