#pragma once

#include "Model/Material/Material.h"
#include "Model/Material/Integrand/SiOpaqueMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

namespace ph
{

class AbradedOpaque : public Material
{
public:
	AbradedOpaque();
	virtual ~AbradedOpaque() override;

	virtual inline const SurfaceIntegrand* getSurfaceIntegrand() const override
	{
		return &m_surfaceIntegrand;
	}

	void setRoughness(const float32 roughness);
	void setF0(const Vector3f& f0);

private:
	SiOpaqueMicrofacet m_surfaceIntegrand;

	float32 m_roughness;
	Vector3f m_f0;
};

}// end namespace ph