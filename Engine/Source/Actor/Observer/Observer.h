#pragma once

#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/sdl_interface.h"

#include <memory>

namespace ph { class CoreCookingContext; }
namespace ph { class CoreCookedUnit; }
namespace ph { class Receiver; }

namespace ph
{

class Observer : public ISdlResource
{
public:
	static constexpr ETypeCategory CATEGORY = ETypeCategory::REF_OBSERVER;

public:
	inline Observer() = default;

	virtual void genReceiver(const CoreCookingContext& ctx, CoreCookedUnit& out_cooked) = 0;

	ETypeCategory getCategory() const override;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<Observer>)
	{
		ClassType clazz("observer");
		clazz.description("A tool for observing the incoming energy of the scene.");
		return clazz;
	}
};

// In-header Implementations:

inline ETypeCategory Observer::getCategory() const
{
	return CATEGORY;
}

}// end namespace ph
