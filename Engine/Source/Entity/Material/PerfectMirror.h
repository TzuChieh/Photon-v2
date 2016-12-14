#pragma once

#include "Entity/Material/Material.h"
#include "Entity/Material/Integrand/SiPerfectMirror.h"

namespace ph
{

class PerfectMirror : public Material
{
public:
	virtual ~PerfectMirror() override;

	virtual const SurfaceIntegrand* getSurfaceIntegrand() const override;

private:
	SiPerfectMirror m_surfaceIntegrand;
};

}// end namespace ph