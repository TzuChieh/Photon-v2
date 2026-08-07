#pragma once

#include "Engine/EngineEnv/CoreSdlResource.h"
#include "Engine/SDL/sdl_interface.h"

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
	PH_DEFINE_SDL_CLASS(Observer, clazz)
	{
		clazz.typeName("observer");
		clazz.docName("Observer");
		clazz.description("A tool for observing the incoming energy of the scene.");
		clazz.addField(makeDisplayNameField<OwnerType>());
	}
};

// In-header Implementations:

inline ESdlTypeCategory Observer::getDynamicCategory() const
{
	return CATEGORY;
}

}// end namespace ph
