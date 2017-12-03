#pragma once

#include "Actor/UvwGenerator/UvwGenerator.h"

namespace ph
{

class SphericalGenerator final : public UvwGenerator
{
public:
	virtual ~SphericalGenerator() override;

	virtual void map(const Vector3R& position, const Vector3R& uvw, Vector3R* const out_uvw) const override;
};

}// end namespace ph