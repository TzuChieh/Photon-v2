#pragma once

#include "EngineEnv/CoreSdlResource.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class SampleSource : public CoreSdlResource
{
public:
	static constexpr ETypeCategory CATEGORY = ETypeCategory::Ref_SampleSource;

public:
	inline SampleSource() = default;

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override = 0;

	ETypeCategory getDynamicCategory() const override;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<SampleSource>)
	{
		ClassType clazz("sample-source");
		clazz.docName("Sample Source");
		clazz.description("Engine component for generating sample values.");
		return clazz;
	}
};

// In-header Implementations:

inline ETypeCategory SampleSource::getDynamicCategory() const
{
	return CATEGORY;
}

}// end namespace ph
