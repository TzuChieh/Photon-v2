#pragma once

#include "EngineEnv/CoreSdlResource.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class Visualizer : public CoreSdlResource
{
public:
	static constexpr ETypeCategory CATEGORY = ETypeCategory::REF_VISUALIZER;

public:
	inline Visualizer() = default;

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override = 0;

	ETypeCategory getCategory() const override;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<Visualizer>)
	{
		ClassType clazz("visualizer");
		clazz.description("The main engine component for producing visual content.");
		return clazz;
	}
};

// In-header Implementations:

inline ETypeCategory Visualizer::getCategory() const
{
	return CATEGORY;
}

}// end namespace ph
