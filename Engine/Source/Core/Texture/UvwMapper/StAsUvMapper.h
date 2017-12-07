#pragma once

#include "Core/Texture/UvwMapper/UvwMapper.h"

namespace ph
{

class StAsUvMapper final : public UvwMapper
{
public:
	virtual ~StAsUvMapper() override;

	virtual void map(const HitDetail& X, Vector3R* out_uvw) const override;
};

}// end namespace ph