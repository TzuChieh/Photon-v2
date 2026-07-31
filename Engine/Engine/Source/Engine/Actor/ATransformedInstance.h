#pragma once

#include "Engine/Actor/PhysicalActor.h"
#include "Engine/SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class ATransformedInstance : public PhysicalActor
{
public:
	PreCookReport preCook(const CookingContext& ctx) const override;
	TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) const override;

	void setSource(const std::shared_ptr<Actor>& source);

private:
	std::shared_ptr<Actor> m_source;

public:
	PH_DEFINE_SDL_CLASS(ATransformedInstance, clazz)
	{
		clazz.typeName("transformed-instance");
		clazz.docName("Transformed Instance Actor");
		clazz.description("Creates a transformed instance of an actor.");
		clazz.baseOn<PhysicalActor>();

		TSdlReference<Actor, OwnerType> source("source", &OwnerType::m_source);
		source.description(
			"The actor to instance. Use is-instantiable-hint to request suitable "
			"output from sources that support instancing. A normal source remains visible. "
			"Declare the source as phantom to show only the instance.");
		source.required();
		clazz.addField(source);
	}
};

}// end namespace ph
