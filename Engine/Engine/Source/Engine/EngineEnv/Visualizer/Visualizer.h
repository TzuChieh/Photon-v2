#pragma once

#include "Engine/EngineEnv/CoreSdlResource.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

class Visualizer : public CoreSdlResource
{
public:
	static constexpr ESdlTypeCategory CATEGORY = ESdlTypeCategory::Ref_Visualizer;

public:
	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override = 0;

	ESdlTypeCategory getDynamicCategory() const override;

public:
	PH_DEFINE_SDL_CLASS(Visualizer, clazz)
	{
		clazz.typeName("visualizer");
		clazz.docName("Visualizer");
		clazz.description("The main engine component for producing visual content.");
		clazz.addField(makeDisplayNameField<OwnerType>());
	}
};

// In-header Implementations:

inline ESdlTypeCategory Visualizer::getDynamicCategory() const
{
	return CATEGORY;
}

}// end namespace ph
