#pragma once

#include "Core/UvwMapper/UvwMapper.h"

namespace ph
{

class SphericalMapper final : public UvwMapper
{
public:
	virtual ~SphericalMapper() override;

	virtual void map(const HitDetail& X, Vector3R* out_uvw) const override;
};

}// end namespace ph