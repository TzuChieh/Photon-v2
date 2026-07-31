#pragma once

#include "Engine/SDL/TSdlResourceBase.h"
#include "Engine/World/Foundation/TransientVisualElement.h"
#include "Engine/SDL/sdl_interface.h"

#include <Common/logging.h>

#include <string>

namespace ph { class PreCookReport; }
namespace ph { class CookingContext; }
namespace ph { class CookOrder; }

namespace ph
{

// General group for actor cooking process
PH_DECLARE_LOG_GROUP(ActorCooking);

class Actor : public TSdlResourceBase<ESdlTypeCategory::Ref_Actor>
{
public:
	/*! @brief Check cookability and prepare dependency-free data for `cook()`.
	This method allows user to specify additional configurations for the cooking process.
	Will run before dependent SDL resources are cooked and may run in parallel.
	*/
	virtual PreCookReport preCook(const CookingContext& ctx) const;

	virtual TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) const = 0;

	/*! @brief Cooking additional data after `cook()`.
	This method provides an opportunity for user to decorate the cooked data or any post-cook
	operations (such as generating debug data). 
	This method may run in parallel without taking into account resource dependencies.
	*/
	virtual void postCook(const CookingContext& ctx, TransientVisualElement& element) const;

	virtual CookOrder getCookOrder() const;

	void setIsInstantiableHint(bool isInstantiable);
	bool isInstantiableHint() const;

	/*! @brief Helper to execute the full cooking procedure in one go.
	*/
	TransientVisualElement stagelessCook(const CookingContext& ctx) const;

private:
	bool m_isInstantiableHint;

public:
	PH_DEFINE_SDL_CLASS(Actor, clazz)
	{
		clazz.typeName(std::string(sdl::category_to_string(CATEGORY)));
		clazz.docName("Actor");
		clazz.description(
			"Represents an entity in the scene. "
			"Every entity that participates in a scene is an actor.");

		TSdlBool<OwnerType> isInstantiableHint("is-instantiable-hint", &OwnerType::m_isInstantiableHint);
		isInstantiableHint.description(
			"Hints that the actor should produce output suitable for use as an instance source.");
		isInstantiableHint.defaultTo(false);
		isInstantiableHint.optional();
		clazz.addField(isInstantiableHint);
	}
};

}// end namespace ph
