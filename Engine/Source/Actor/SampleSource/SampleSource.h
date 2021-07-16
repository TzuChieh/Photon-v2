#pragma once

#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph { class CoreCookingContext; }
namespace ph { class CoreCookedUnit; }

namespace ph
{

class SampleSource : public ISdlResource
{
public:
	static constexpr ETypeCategory CATEGORY = ETypeCategory::REF_SAMPLE_SOURCE;

public:
	inline SampleSource() = default;

	virtual void cook(const CoreCookingContext& ctx, CoreCookedUnit& out_cooked) = 0;

	ETypeCategory getCategory() const override;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<SampleSource>)
	{
		ClassType clazz("sample-source");
		clazz.description("Engine component for generating sample values.");
		return clazz;
	}
};

// In-header Implementations:

inline ETypeCategory Observer::getCategory() const
{
	return CATEGORY;
}

}// end namespace ph
