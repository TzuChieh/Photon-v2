#pragma once

#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph { class CoreCookingContext; }
namespace ph { class CoreCookedUnit; }

namespace ph
{

class Visualizer : public ISdlResource
{
public:
	static constexpr ETypeCategory CATEGORY = ETypeCategory::REF_VISUALIZER;

public:
	inline Visualizer() = default;

	virtual void cook(const CoreCookingContext& ctx, CoreCookedUnit& out_cooked) = 0;

	ETypeCategory getCategory() const override;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<Visualizer>)
	{
		ClassType clazz("visualizer");
		clazz.description("The main engine component for producing images.");
		return clazz;
	}
};

// In-header Implementations:

inline ETypeCategory Observer::getCategory() const
{
	return CATEGORY;
}

}// end namespace ph
