#pragma once

#include "SDL/TSdlResourceBase.h"
#include "World/Foundation/TransientVisualElement.h"
#include "SDL/sdl_interface.h"

#include <string>

namespace ph { class PreCookReport; }
namespace ph { class CookingContext; }
namespace ph { class CookOrder; }

namespace ph
{

class Actor : public TSdlResourceBase<ESdlTypeCategory::Ref_Actor>
{
public:
	/*! @brief Cooking supplemental data before `cook()`.
	This method allows user to specify additional configurations for the following cooking process.
	This method may run in parallel without taking into account resource dependencies.
	*/
	virtual PreCookReport preCook(CookingContext& ctx) const;

	virtual TransientVisualElement cook(CookingContext& ctx, const PreCookReport& report) = 0;

	/*! @brief Cooking additional data after `cook()`.
	This method provides an opportunity for user to decorate the cooked data or any post-cook
	operations (such as generating debug data). 
	This method may run in parallel without taking into account resource dependencies.
	*/
	virtual void postCook(const CookingContext& ctx, TransientVisualElement& element) const;

	virtual CookOrder getCookOrder() const;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<Actor>)
	{
		ClassType clazz(std::string(sdl::category_to_string(CATEGORY)));
		clazz.docName("Actor");
		clazz.description(
			"Represents an entity in the scene. "
			"Every entity that participates in a scene is an actor.");
		return clazz;
	}
};

}// end namespace ph
