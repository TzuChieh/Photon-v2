#pragma once

#include "Engine/Actor/Basic/TransformInfo.h"
#include "Engine/Actor/PhysicalActor.h"
#include "Engine/SDL/sdl_interface.h"

#include <memory>
#include <vector>

namespace ph
{

class ATransformedInstance : public PhysicalActor
{
public:
	PreCookReport preCook(const CookingContext& ctx) const override;
	TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) const override;

	void setSource(const std::shared_ptr<Actor>& source);
	void setTransforms(std::vector<TransformInfo> transforms);

private:
	std::shared_ptr<Actor> m_source;
	std::vector<TransformInfo> m_transforms;

public:
	PH_DEFINE_SDL_CLASS(ATransformedInstance, clazz)
	{
		clazz.typeName("transformed-instance");
		clazz.docName("Transformed Instance Actor");
		clazz.description("Creates transformed instances of an actor.");
		clazz.baseOn<PhysicalActor>();

		TSdlReference<Actor, OwnerType> source("source", &OwnerType::m_source);
		source.description(
			"The actor to instance. Use is-instantiable-hint to request suitable "
			"output from sources that support instancing. A normal source remains visible. "
			"Declare the source as phantom to show only the instance.");
		source.required();
		clazz.addField(source);

		TSdlStructArray<TransformInfo, OwnerType> transforms("transforms", &OwnerType::m_transforms);
		transforms.description(
			"Per-instance transforms applied after the actor transform. "
			"An empty array creates no instance.");
		transforms.optional();
		clazz.addField(transforms);
	}
};

}// end namespace ph
