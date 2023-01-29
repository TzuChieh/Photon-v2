#pragma once

#include "DataIO/SDL/TSdlResourceBase.h"
#include "World/Foundation/CookedUnit.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph { class PreCookReport; }
namespace ph { class CookingContext; }
namespace ph { class CookOrder; }

namespace ph
{

class Actor : public TSdlResourceBase<ETypeCategory::Ref_Actor>
{
public:
	virtual CookedUnit cook(CookingContext& ctx, const PreCookReport& report) = 0;

	/*! @brief Cooking supplemental data before `cook()`.
	This method allows user to specify additional configurations for the following cooking process.
	The method is guaranteed to run in parallel.
	*/
	virtual PreCookReport preCook(CookingContext& ctx);

	/*! @brief Adjust cooked unit after `cook()`.
	This method allows user to decorate (perform additional modifications to) the cooked data.
	The method is guaranteed to run in parallel.
	*/
	virtual void postCook(CookingContext& ctx, CookedUnit& cookedUnit);

	virtual CookOrder getCookOrder() const;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<Actor>)
	{
		ClassType clazz(sdl::category_to_string(CATEGORY));
		clazz.docName("Actor");
		clazz.description(
			"Represents an entity in the scene. "
			"Every entity that participates in a scene is an actor.");
		return clazz;
	}
};

}// end namespace ph
