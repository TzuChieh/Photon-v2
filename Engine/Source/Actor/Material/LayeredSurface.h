#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Actor/Material/Utility/SurfaceLayerInfo.h"

#include <vector>

namespace ph
{

class LayeredSurface : public SurfaceMaterial
{
public:
	LayeredSurface();

	void genSurface(ActorCookingContext& ctx, SurfaceBehavior& behavior) const override;
	
	void addLayer();
	void setLayer(std::size_t layerIndex, const SurfaceLayerInfo& layer);

private:
	std::vector<SurfaceLayerInfo> m_layers;
};

}// end namespace ph
