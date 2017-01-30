#pragma once

#include "Actor/TextureMapper/TextureMapper.h"

namespace ph
{

class DefaultMapper final : public TextureMapper
{
public:
	virtual ~DefaultMapper() override;

	virtual void map(const Vector3R& position, const Vector3R& uvw, Vector3R* const out_uvw) const override;
};

}// end namespace ph