#pragma once

#include "EngineEnv/CoreSdlResource.h"
#include "SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class Observer : public CoreSdlResource
{
public:
	static constexpr ESdlTypeCategory CATEGORY = ESdlTypeCategory::Ref_Observer;

public:
	inline Observer() = default;

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override = 0;

	ESdlTypeCategory getDynamicCategory() const override;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<Observer>)
	{
		ClassType clazz("observer");
		clazz.docName("Observer");
		clazz.description("A tool for observing the incoming energy of the scene.");
		return clazz;
	}
};

// In-header Implementations:

inline ESdlTypeCategory Observer::getDynamicCategory() const
{
	return CATEGORY;
}

}// end namespace ph
