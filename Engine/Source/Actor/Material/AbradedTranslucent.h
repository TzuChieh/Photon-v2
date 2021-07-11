#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Actor/Material/Utility/DielectricInterfaceInfo.h"
#include "Actor/Material/Utility/MicrosurfaceInfo.h"

namespace ph
{

class AbradedTranslucent : public SurfaceMaterial
{
public:
	AbradedTranslucent();

	void genSurface(ActorCookingContext& ctx, SurfaceBehavior& behavior) const override;

private:
	DielectricInterfaceInfo m_interfaceInfo;
	MicrosurfaceInfo        m_microsurfaceInfo;
};

}// end namespace ph
