#pragma once

#include "Entity/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

#include <vector>

namespace ph
{

class GSphere final : public Geometry
{
public:
	GSphere(const float32 radius);
	GSphere(const GSphere& other);
	virtual ~GSphere() override;

	virtual void discretize(std::vector<Triangle>* const out_triangles, const Entity* const parentEntity) const override;

	GSphere& operator = (const GSphere& rhs);

private:
	float32 m_radius;

	std::size_t addVertex(const Vector3f& vertex, std::vector<Vector3f>* const out_vertices) const;
	std::size_t addMidpointVertex(const std::size_t iA, const std::size_t iB, std::vector<Vector3f>* const out_vertices) const;
};

}// end namespace ph