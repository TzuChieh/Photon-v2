#pragma once

#include "Model/TextureMapper/TextureMapper.h"

namespace ph
{

class SphericalMapper final : public TextureMapper
{
public:
	virtual ~SphericalMapper() override;

	virtual void map(const Vector3f& position, const Vector3f& uvw, Vector3f* const out_uvw) const override;
};

}// end namespace ph