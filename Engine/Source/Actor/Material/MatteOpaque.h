#pragma once

#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/LambertianDiffuse.h"
#include "Math/Vector3f.h"
#include "Image/Texture.h"

#include <memory>

namespace ph
{

class MatteOpaque : public Material
{
public:
	MatteOpaque();
	virtual ~MatteOpaque() override;

	virtual void populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const override;

	void setAlbedo(const Vector3f& albedo);
	void setAlbedo(const float32 r, const float32 g, const float32 b);
	void setAlbedo(const std::shared_ptr<Texture>& albedo);

private:
	LambertianDiffuse m_bsdfCos;
};

}// end namespace ph