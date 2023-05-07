#pragma once

#include "SDL/TSdlResourceBase.h"
#include "SDL/sdl_interface.h"

namespace ph
{

class Object : public TSdlResourceBase<ESdlTypeCategory::Ref_Object>
{
public:
	Object();
	~Object() override = 0;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<Object>)
	{
		ClassType clazz(sdl::category_to_string(CATEGORY));
		clazz.docName("Object");
		clazz.description("General object that may refer to any type.");
		return clazz;
	}
};

}// end namespace ph
