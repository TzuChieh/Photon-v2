#pragma once

#include "Core/Intersection/UvwMapper/UvwMapper.h"

#include <Common/assertion.h>

namespace ph
{

class SphericalMapper : public UvwMapper
{
public:
	void posToUvw(const math::Vector3R& pos, math::Vector3R* out_uvw) const override;
	void dirToUvw(const math::Vector3R& dir, math::Vector3R* out_uvw) const override;
	bool uvwToPos(const math::Vector3R& uvw, math::Vector3R* out_pos) const override;
	bool uvwToDir(const math::Vector3R& uvw, math::Vector3R* out_dir) const override;
};

// In-header Implementations:

inline void SphericalMapper::posToUvw(const math::Vector3R& pos, math::Vector3R* const out_uvw) const
{
	PH_ASSERT(out_uvw);

	// HACK: magic number
	if(pos.lengthSquared() < 1e-8)
	{
		out_uvw->set({0, 0, 0});
		return;
	}

	dirToUvw(pos, out_uvw);
}

inline bool SphericalMapper::uvwToPos(const math::Vector3R& uvw, math::Vector3R* const out_pos) const
{
	return false;
}

}// end namespace ph
