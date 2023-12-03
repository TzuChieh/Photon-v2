#pragma once

#include "Core/HitDetail.h"
#include "Math/TVector3.h"

namespace ph
{

class UvwMapper
{
public:
	virtual ~UvwMapper();

	virtual void positionToUvw(const math::Vector3R& position, math::Vector3R* out_uvw) const = 0;

	virtual void directionToUvw(const math::Vector3R& direction, math::Vector3R* out_uvw) const;
	virtual bool uvwToPosition(const math::Vector3R& uvw, math::Vector3R* out_position) const;
	virtual bool uvwToDirection(const math::Vector3R& uvw, math::Vector3R* out_direction) const;
};

// In-header Implementations:

inline void UvwMapper::directionToUvw(const math::Vector3R& direction, math::Vector3R* const out_uvw) const
{
	positionToUvw(direction, out_uvw);
}

inline bool UvwMapper::uvwToPosition(const math::Vector3R& uvw, math::Vector3R* const out_position) const
{
	return false;
}

inline bool UvwMapper::uvwToDirection(const math::Vector3R& uvw, math::Vector3R* const out_direction) const
{
	return false;
}

}// end namespace ph
