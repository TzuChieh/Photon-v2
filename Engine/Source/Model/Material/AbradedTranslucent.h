#pragma once

#include "Model/Material/Material.h"
#include "Model/Material/Integrand/SiTranslucentMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

namespace ph
{

class AbradedTranslucent : public Material
{
public:
	AbradedTranslucent();
	virtual ~AbradedTranslucent() override;

	virtual inline const SurfaceIntegrand* getSurfaceIntegrand() const override
	{
		return &m_surfaceIntegrand;
	}

	void setF0(const Vector3f f0);
	void setF0(const float32 r, const float32 g, const float32 b);
	void setIOR(const float32 ior);
	void setRoughness(const float32 roughness);

private:
	SiTranslucentMicrofacet m_surfaceIntegrand;
};

}// end namespace ph