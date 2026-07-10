#pragma once

#include "Engine/Actor/Material/SurfaceMaterial.h"
#include "Engine/Actor/Material/Component/SurfaceLayerInfo.h"
#include "Engine/SDL/sdl_interface.h"

#include <vector>

namespace ph
{

class LayeredSurface : public SurfaceMaterial
{
public:
	void storeCooked(
		const CookingContext& ctx,
		CookedMaterial& out_material) const override;
	
	void addLayer();
	void setLayer(std::size_t layerIndex, const SurfaceLayerInfo& layer);

private:
	std::vector<SurfaceLayerInfo> m_layers;

public:
	PH_DEFINE_SDL_CLASS(LayeredSurface, clazz)
	{
		clazz.typeName("layered-surface");
		clazz.docName("Layered Surface");
		clazz.description(
			"Model a surface as having multiple coating layers. For paired value/map "
			"inputs in each layer, map inputs have higher precedence. If any map is "
			"specified for a layer, its constant inputs are promoted to constant textures.");
		clazz.baseOn<SurfaceMaterial>();

		TSdlStructArray<SurfaceLayerInfo, OwnerType> layers("layers", &OwnerType::m_layers);
		layers.description("Physical properties of each layer.");
		layers.required();
		clazz.addField(layers);
	}
};

}// end namespace ph
