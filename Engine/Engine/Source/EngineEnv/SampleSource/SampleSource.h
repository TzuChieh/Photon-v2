#pragma once

#include "EngineEnv/CoreSdlResource.h"
#include "SDL/sdl_interface.h"

namespace ph
{

class SampleSource : public CoreSdlResource
{
public:
	static constexpr ESdlTypeCategory CATEGORY = ESdlTypeCategory::Ref_SampleSource;

public:
	inline SampleSource() = default;

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override = 0;

	ESdlTypeCategory getDynamicCategory() const override;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<SampleSource>)
	{
		ClassType clazz("sample-source");
		clazz.docName("Sample Source");
		clazz.description("Engine component for generating sample values.");
		return clazz;
	}
};

// In-header Implementations:

inline ESdlTypeCategory SampleSource::getDynamicCategory() const
{
	return CATEGORY;
}

}// end namespace ph
