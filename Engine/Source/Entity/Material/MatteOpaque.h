#pragma once

#include "Entity/Material/Material.h"
#include "Entity/Material/Integrand/SiLambertianDiffuse.h"
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

	virtual inline const SurfaceIntegrand* getSurfaceIntegrand() const override
	{
		return &m_surfaceIntegrand;
	}

	void setAlbedo(const Vector3f& albedo);
	void setAlbedo(const float32 r, const float32 g, const float32 b);
	void setAlbedo(const std::shared_ptr<Texture>& albedo);

private:
	SiLambertianDiffuse m_surfaceIntegrand;
};

}// end namespace ph