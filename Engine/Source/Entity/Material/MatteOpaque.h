#pragma once

#include "Entity/Material/Material.h"
#include "Entity/Material/SurfaceBehavior/SLambertianDiffuse.h"
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

	virtual inline const SurfaceBehavior* getSurfaceBehavior() const override
	{
		return &m_surfaceBehavior;
	}

	void setAlbedo(const Vector3f& albedo);
	void setAlbedo(const float32 r, const float32 g, const float32 b);
	void setAlbedo(const std::shared_ptr<Texture>& albedo);

private:
	SLambertianDiffuse m_surfaceBehavior;
};

}// end namespace ph