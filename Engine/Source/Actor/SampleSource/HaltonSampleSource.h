#pragma once

#include "Actor/SampleSource/RuntimeSampleSource.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class HaltonSampleSource : public RuntimeSampleSource
{
public:
	inline HaltonSampleSource() = default;

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& out_cooked) override;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<HaltonSampleSource>)
	{
		ClassType clazz("halton");
		clazz.description(
			"Generating samples based on the Halton sequence. The samples "
			"generated are somewhat deterministic and can lead to visible "
			"patterns if the number of samples is too low.");
		clazz.baseOn<RuntimeSampleSource>();
		return clazz;
	}
};

}// end namespace ph
