#pragma once

#include "EngineEnv/CoreSdlResource.h"
#include "SDL/sdl_interface.h"

namespace ph
{

class Visualizer : public CoreSdlResource
{
public:
	static constexpr ESdlTypeCategory CATEGORY = ESdlTypeCategory::Ref_Visualizer;

public:
	inline Visualizer() = default;

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override = 0;

	ESdlTypeCategory getDynamicCategory() const override;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<Visualizer>)
	{
		ClassType clazz("visualizer");
		clazz.docName("Visualizer");
		clazz.description("The main engine component for producing visual content.");
		return clazz;
	}
};

// In-header Implementations:

inline ESdlTypeCategory Visualizer::getDynamicCategory() const
{
	return CATEGORY;
}

}// end namespace ph
