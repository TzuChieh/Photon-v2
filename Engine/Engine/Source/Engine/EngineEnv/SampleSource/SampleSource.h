#pragma once

#include "Engine/EngineEnv/CoreSdlResource.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

class SampleSource : public CoreSdlResource
{
public:
	static constexpr ESdlTypeCategory CATEGORY = ESdlTypeCategory::Ref_SampleSource;

public:
	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override = 0;

	ESdlTypeCategory getDynamicCategory() const override;

public:
	PH_DEFINE_SDL_CLASS(SampleSource, clazz)
	{
		clazz.typeName("sample-source");
		clazz.docName("Sample Source");
		clazz.description("Engine component for generating sample values.");
	}
};

// In-header Implementations:

inline ESdlTypeCategory SampleSource::getDynamicCategory() const
{
	return CATEGORY;
}

}// end namespace ph
