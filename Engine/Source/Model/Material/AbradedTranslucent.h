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

private:
	SiTranslucentMicrofacet m_surfaceIntegrand;
};

}// end namespace ph