#pragma once

#include "Core/HitDetail.h"
#include "Math/TVector3.h"

namespace ph
{

class UvwMapper
{
public:
	virtual ~UvwMapper();

	virtual void positionToUvw(const Vector3R& position, Vector3R* out_uvw) const = 0;

	virtual void directionToUvw(const Vector3R& direction, Vector3R* out_uvw) const;
	virtual bool uvwToPosition(const Vector3R& uvw, Vector3R* out_position) const;
	virtual bool uvwToDirection(const Vector3R& uvw, Vector3R* out_direction) const;
};

// In-header Implementations:

inline void UvwMapper::directionToUvw(const Vector3R& direction, Vector3R* const out_uvw) const
{
	positionToUvw(direction, out_uvw);
}

inline bool UvwMapper::uvwToPosition(const Vector3R& uvw, Vector3R* const out_position) const
{
	return false;
}

inline bool UvwMapper::uvwToDirection(const Vector3R& uvw, Vector3R* const out_direction) const
{
	return false;
}

}// end namespace ph