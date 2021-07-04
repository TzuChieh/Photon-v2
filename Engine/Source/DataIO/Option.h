#pragma once

#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class Option : public ISdlResource
{
public:
	static constexpr ETypeCategory CATEGORY = ETypeCategory::REF_OPTION;

	inline Option() = default;
	virtual inline ~Option() = default;

	ETypeCategory getCategory() const override;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<Option>)
	{
		ClassType clazz(sdl::category_to_string(CATEGORY));
		clazz.description(
			"Options that control engine runtime behavior.");
		return clazz;
	}
};

// In-header Implementations:

inline ETypeCategory Option::getCategory() const
{
	return CATEGORY;
}

}// end namespace ph
