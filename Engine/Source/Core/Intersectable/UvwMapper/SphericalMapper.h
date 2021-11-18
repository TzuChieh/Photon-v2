#pragma once

#include "Core/Intersectable/UvwMapper/UvwMapper.h"
#include "Common/assertion.h"

namespace ph
{

class SphericalMapper : public UvwMapper
{
public:
	void positionToUvw(const math::Vector3R& position, math::Vector3R* out_uvw) const override;
	void directionToUvw(const math::Vector3R& direction, math::Vector3R* out_uvw) const override;
	bool uvwToPosition(const math::Vector3R& uvw, math::Vector3R* out_position) const override;
	bool uvwToDirection(const math::Vector3R& uvw, math::Vector3R* out_direction) const override;
};

// In-header Implementations:

inline void SphericalMapper::positionToUvw(const math::Vector3R& position, math::Vector3R* const out_uvw) const
{
	PH_ASSERT(out_uvw);

	// HACK: magic number
	if(position.lengthSquared() < 1e-8)
	{
		out_uvw->set({0, 0, 0});
		return;
	}

	directionToUvw(position, out_uvw);
}

inline bool SphericalMapper::uvwToPosition(const math::Vector3R& uvw, math::Vector3R* const out_position) const
{
	return false;
}

}// end namespace ph
