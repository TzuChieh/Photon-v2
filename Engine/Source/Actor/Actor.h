#pragma once

#include "DataIO/SDL/TSdlResourceBase.h"
#include "World/Foundation/TransientVisualElement.h"
#include "DataIO/SDL/sdl_interface.h"

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
	The method is guaranteed to run in parallel.
	*/
	virtual PreCookReport preCook(CookingContext& ctx);

	virtual TransientVisualElement cook(CookingContext& ctx, const PreCookReport& report) = 0;

	/*! @brief Cooking additional data after `cook()`.
	This method provides an opportunity for user to decorate the cooked data or any post-cook
	operations (such as generating debug data). 
	The method is guaranteed to run in parallel.
	*/
	virtual void postCook(const CookingContext& ctx, const TransientVisualElement& element) const;

	virtual CookOrder getCookOrder() const;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<Actor>)
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
