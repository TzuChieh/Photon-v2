#pragma once

#include "Engine/Actor/PhysicalActor.h"
#include "Engine/Actor/Geometry/Geometry.h"
#include "Engine/Actor/Material/Material.h"
#include "Engine/SDL/sdl_interface.h"

#include <memory>
#include <vector>

namespace ph
{

class ABlenderPlyModel : public PhysicalActor
{
public:
	PreCookReport preCook(const CookingContext& ctx) const override;
	TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) const override;

	void setGeometry(const std::shared_ptr<Geometry>& geometry);
	void setMaterials(std::vector<std::shared_ptr<Material>> materials);
	void setShouldFlipNg(bool shouldFlipNg);
	bool shouldFlipNg() const;

private:
	std::shared_ptr<Geometry> m_geometry;
	std::vector<std::shared_ptr<Material>> m_materials;
	bool m_shouldFlipNg = false;

public:
	PH_DEFINE_SDL_CLASS(ABlenderPlyModel, clazz)
	{
		clazz.typeName("blender-ply-model");
		clazz.docName("Blender PLY Model Actor");
		clazz.description("An exporter-private actor for Blender PLY meshes with material slots.");
		clazz.baseOn<PhysicalActor>();

		TSdlReference<Geometry, OwnerType> geometry("geometry", &OwnerType::m_geometry);
		geometry.description("A Blender PLY geometry that represents this actor's shape.");
		geometry.required();
		clazz.addField(geometry);

		TSdlReferenceArray<Material, OwnerType> materials("materials", &OwnerType::m_materials);
		materials.description("Materials indexed by Blender material slot.");
		materials.required();
		clazz.addField(materials);

		TSdlBool<OwnerType> shouldFlipNg("should-flip-ng", &OwnerType::m_shouldFlipNg);
		shouldFlipNg.description(
			"Flips only the geometric normal (Ng) after trasnform; the shading normal (Ns) is not flipped.");
		shouldFlipNg.defaultTo(false);
		shouldFlipNg.optional();
		clazz.addField(shouldFlipNg);
	}
};

}// end namespace ph
