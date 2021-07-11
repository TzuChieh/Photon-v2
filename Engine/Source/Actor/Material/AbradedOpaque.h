#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Actor/Material/Utility/ConductiveInterfaceInfo.h"
#include "Actor/Material/Utility/MicrosurfaceInfo.h"

namespace ph
{

class AbradedOpaque : public SurfaceMaterial
{
public:
	AbradedOpaque();

	void genSurface(ActorCookingContext& ctx, SurfaceBehavior& behavior) const override;

private:
	ConductiveInterfaceInfo m_interfaceInfo;
	MicrosurfaceInfo        m_microsurfaceInfo;
};

}// end namespace ph
