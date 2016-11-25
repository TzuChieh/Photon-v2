#pragma once

#include "Model/TextureMapper/TextureMapper.h"

namespace ph
{

class DefaultMapper final : public TextureMapper
{
public:
	virtual ~DefaultMapper() override;

	virtual void map(const Vector3f& position, const float32 u, const float32 v, float32* const out_u, float32* const out_v) const override;
};

}// end namespace ph