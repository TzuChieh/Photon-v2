#pragma once

#include "Engine/SDL/TSdlResourceBase.h"
#include "Engine/SDL/sdl_interface.h"

#include <string>

namespace ph
{

class Object : public TSdlResourceBase<ESdlTypeCategory::Ref_Object>
{
protected:
	Object();
	Object(const Object& other);
	Object(Object&& other) noexcept;
	~Object() override = 0;

	Object& operator = (const Object& rhs);
	Object& operator = (Object&& rhs) noexcept;

public:
	PH_DEFINE_SDL_CLASS(Object, clazz)
	{
		clazz.typeName(std::string(sdl::category_to_string(CATEGORY)));
		clazz.docName("Object");
		clazz.description("General object that may refer to any type.");
		clazz.addField(makeDisplayNameField<OwnerType>());
	}
};

}// end namespace ph
