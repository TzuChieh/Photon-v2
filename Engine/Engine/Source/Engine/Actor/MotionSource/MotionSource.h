#pragma once

#include "Engine/SDL/TSdlResourceBase.h"
#include "Engine/SDL/sdl_interface.h"

#include <string>

namespace ph
{

class CookingContext;
class CookedMotion;

class MotionSource : public TSdlResourceBase<ESdlTypeCategory::Ref_Motion>
{
public:
	/*! @brief Store data suitable for rendering into `out_motion`.
	*/
	virtual void storeCooked(
		const CookingContext& ctx,
		CookedMotion& out_motion) const = 0;

	/*! @brief Cook motion using the provided context and output storage.
	*/
	void cook(
		const CookingContext& ctx,
		CookedMotion& out_motion) const;

public:
	PH_DEFINE_SDL_CLASS(MotionSource, clazz)
	{
		clazz.typeName(std::string(sdl::category_to_string(CATEGORY)));
		clazz.docName("Motion Source");
		clazz.description("Describes scene element movement over time.");
		clazz.addField(makeDisplayNameField<OwnerType>());
	}
};

}// end namespace ph
