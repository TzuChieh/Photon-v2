#pragma once

#include "Actor/PhysicalActor.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class AObserver : public PhysicalActor
{
public:
	AObserver();
	AObserver(const AObserver& other);

	CookedUnit cook(CookingContext& context) override = 0;

	CookOrder getCookOrder() const override;

	AObserver& operator = (const AObserver& rhs);

	friend void swap(AObserver& first, AObserver& second);

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<AObserver>)
	{
		ClassType clazz("observer");
		clazz.description("An actor that is visible and can be transformed.");
		clazz.baseOn<PhysicalActor>();

		clazz.addFunction<SdlTranslate>();
		clazz.addFunction<SdlRotate>();
		clazz.addFunction<SdlScale>();

		return clazz;
	}
};

// In-header Implementations:

inline CookOrder AObserver::getCookOrder() const
{
	return CookOrder(ECookPriority::HIGH, ECookLevel::FIRST);
}

}// end namespace ph
