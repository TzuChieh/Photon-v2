#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Actor/Material/Component/SurfaceLayerInfo.h"
#include "SDL/sdl_interface.h"

#include <vector>

namespace ph
{

class LayeredSurface : public SurfaceMaterial
{
public:
	LayeredSurface();

	void genSurface(const CookingContext& ctx, SurfaceBehavior& behavior) const override;
	
	void addLayer();
	void setLayer(std::size_t layerIndex, const SurfaceLayerInfo& layer);

private:
	std::vector<SurfaceLayerInfo> m_layers;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<LayeredSurface>)
	{
		ClassType clazz("layered-surface");
		clazz.docName("Layered Surface");
		clazz.description("Model a surface as having multiple coating layers.");
		clazz.baseOn<SurfaceMaterial>();

		/*clazz.addStruct(&OwnerType::m_interfaceInfo);
		clazz.addStruct(&OwnerType::m_microsurfaceInfo);*/

		return clazz;
	}
};

}// end namespace ph
