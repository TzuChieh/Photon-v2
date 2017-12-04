#pragma once

#include "Actor/StGenerator/StGenerator.h"

namespace ph
{

class DefaultGenerator final : public StGenerator
{
public:
	virtual ~DefaultGenerator() override;

	virtual void map(const Vector3R& position, const Vector3R& uvw, Vector3R* const out_uvw) const override;
};

}// end namespace ph