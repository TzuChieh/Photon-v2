#pragma once

#include "EngineEnv/ICoreSdlResource.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class SampleSource : public ICoreSdlResource
{
public:
	static constexpr ETypeCategory CATEGORY = ETypeCategory::REF_SAMPLE_SOURCE;

public:
	inline SampleSource() = default;

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override = 0;

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

inline ETypeCategory SampleSource::getCategory() const
{
	return CATEGORY;
}

}// end namespace ph
