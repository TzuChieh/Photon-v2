#pragma once

#include "Actor/UvwGenerator/UvwGenerator.h"

namespace ph
{

class DefaultGenerator final : public UvwGenerator
{
public:
	virtual ~DefaultGenerator() override;

	virtual void map(const Vector3R& position, const Vector3R& uvw, Vector3R* const out_uvw) const override;
};

}// end namespace ph