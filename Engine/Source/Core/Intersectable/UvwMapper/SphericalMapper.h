#pragma once

#include "Core/Intersectable/UvwMapper/UvwMapper.h"

namespace ph
{

class SphericalMapper final : public UvwMapper
{
public:
	virtual ~SphericalMapper() override;

	virtual void map(const Vector3R& vector, Vector3R* out_uvw) const override;
};

}// end namespace ph