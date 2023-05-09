#pragma once

#include "SDL/TSdlResourceBase.h"
#include "SDL/sdl_interface.h"

#include <string>

namespace ph
{

class Option : public TSdlResourceBase<ESdlTypeCategory::Ref_Option>
{
public:
	inline Option() = default;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<Option>)
	{
		ClassType clazz(std::string(sdl::category_to_string(CATEGORY)));
		clazz.docName("Option");
		clazz.description(
			"Options that control engine runtime behavior.");
		return clazz;
	}
};

}// end namespace ph
