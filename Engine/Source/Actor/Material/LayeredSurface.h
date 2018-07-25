#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Actor/Material/Utility/SurfaceLayerProperty.h"

#include <vector>

namespace ph
{

class LayeredSurface : public SurfaceMaterial, public TCommandInterface<LayeredSurface>
{
public:
	LayeredSurface();

	void genSurface(CookingContext& context, SurfaceBehavior& behavior) const override;

private:
	std::vector<SurfaceLayerProperty> m_layers;

// command interface
public:
	explicit LayeredSurface(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph