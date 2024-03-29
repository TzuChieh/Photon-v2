#pragma once

#include "Actor/PhysicalActor.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Actor/MotionSource/MotionSource.h"
#include "SDL/sdl_interface.h"

#include <memory>
#include <vector>

namespace ph
{

class AModel : public PhysicalActor
{
public:
	PreCookReport preCook(const CookingContext& ctx) const override;
	TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) const override;

	const Geometry* getGeometry() const;
	const Material* getMaterial() const;
	void setGeometry(const std::shared_ptr<Geometry>& geometry);
	void setMaterial(const std::shared_ptr<Material>& material);
	void setMotionSource(const std::shared_ptr<MotionSource>& motion);

private:
	std::shared_ptr<Geometry>     m_geometry;
	std::shared_ptr<Material>     m_material;
	std::shared_ptr<MotionSource> m_motionSource;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<AModel>)
	{
		ClassType clazz("model");
		clazz.docName("Model Actor");
		clazz.description("An actor that has a certain 3-D shape in the scene.");
		clazz.baseOn<PhysicalActor>();

		TSdlReference<Geometry, OwnerType> geometry("geometry", &OwnerType::m_geometry);
		geometry.description("A geometry that represent this actor's shape.");
		geometry.required();
		clazz.addField(geometry);

		TSdlReference<Material, OwnerType> material("material", &OwnerType::m_material);
		material.description("A material that describes this actor's surface appearance.");
		material.required();
		clazz.addField(material);

		TSdlReference<MotionSource, OwnerType> motion("motion", &OwnerType::m_motionSource);
		motion.description("Movement of this actor.");
		motion.optional();
		clazz.addField(motion);

		return clazz;
	}
};

}// end namespace ph
