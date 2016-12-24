#pragma once

#include "Entity/Material/Material.h"
#include "Entity/Material/SurfaceBehavior/SPerfectMirror.h"

namespace ph
{

class PerfectMirror : public Material
{
public:
	virtual ~PerfectMirror() override;

	virtual const SurfaceBehavior* getSurfaceBehavior() const override;

private:
	SPerfectMirror m_surfaceBehavior;
};

}// end namespace ph