#pragma once

#include "Core/Intersectable/UvwMapper/UvwMapper.h"

namespace ph
{

class SphericalMapper : public UvwMapper
{
public:
	void positionToUvw(const Vector3R& position, Vector3R* out_uvw) const override;
};

}// end namespace ph