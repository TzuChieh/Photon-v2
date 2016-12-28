#pragma once

#include "Actor/TextureMapper/TextureMapper.h"

namespace ph
{

class DefaultMapper final : public TextureMapper
{
public:
	virtual ~DefaultMapper() override;

	virtual void map(const Vector3f& position, const Vector3f& uvw, Vector3f* const out_uvw) const override;
};

}// end namespace ph