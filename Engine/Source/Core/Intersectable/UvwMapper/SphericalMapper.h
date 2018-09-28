#pragma once

#include "Core/Intersectable/UvwMapper/UvwMapper.h"
#include "Common/assertion.h"

namespace ph
{

class SphericalMapper : public UvwMapper
{
public:
	void positionToUvw(const Vector3R& position, Vector3R* out_uvw) const override;
	void directionToUvw(const Vector3R& direction, Vector3R* out_uvw) const;
	bool uvwToPosition(const Vector3R& uvw, Vector3R* out_position) const;
	bool uvwToDirection(const Vector3R& uvw, Vector3R* out_direction) const;
};

// In-header Implementations:

inline void SphericalMapper::positionToUvw(const Vector3R& position, Vector3R* const out_uvw) const
{
	PH_ASSERT(out_uvw);

	// HACK: magic number
	if(position.lengthSquared() < 1e-8)
	{
		out_uvw->set(0, 0, 0);
		return;
	}

	directionToUvw(position, out_uvw);
}

inline bool SphericalMapper::uvwToPosition(const Vector3R& uvw, Vector3R* const out_position) const
{
	return false;
}

}// end namespace ph