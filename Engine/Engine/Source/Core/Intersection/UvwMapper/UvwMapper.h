#pragma once

#include "Core/HitDetail.h"
#include "Math/TVector3.h"

namespace ph
{

class UvwMapper
{
public:
	virtual ~UvwMapper();

	virtual void posToUvw(const math::Vector3R& pos, math::Vector3R* out_uvw) const = 0;

	virtual void dirToUvw(const math::Vector3R& dir, math::Vector3R* out_uvw) const;
	virtual bool uvwToPos(const math::Vector3R& uvw, math::Vector3R* out_pos) const;
	virtual bool uvwToDir(const math::Vector3R& uvw, math::Vector3R* out_dir) const;
};

// In-header Implementations:

inline void UvwMapper::dirToUvw(const math::Vector3R& dir, math::Vector3R* const out_uvw) const
{
	posToUvw(dir, out_uvw);
}

inline bool UvwMapper::uvwToPos(const math::Vector3R& uvw, math::Vector3R* const out_pos) const
{
	return false;
}

inline bool UvwMapper::uvwToDir(const math::Vector3R& uvw, math::Vector3R* const out_dir) const
{
	return false;
}

}// end namespace ph
