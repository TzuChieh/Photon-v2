#pragma once

#include "SDL/TSdlResourceBase.h"
#include "SDL/sdl_interface.h"

#include <string>

namespace ph
{

class Object : public TSdlResourceBase<ESdlTypeCategory::Ref_Object>
{
public:
	Object();
	Object(const Object& other);
	Object(Object&& other);
	~Object() override = 0;

	Object& operator = (const Object& rhs);
	Object& operator = (Object&& rhs);

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<Object>)
	{
		ClassType clazz(std::string(sdl::category_to_string(CATEGORY)));
		clazz.docName("Object");
		clazz.description("General object that may refer to any type.");
		return clazz;
	}
};

}// end namespace ph
