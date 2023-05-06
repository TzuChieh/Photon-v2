#pragma once

#include "EngineEnv/SampleSource/RuntimeSampleSource.h"
#include "SDL/sdl_interface.h"

namespace ph
{

class HaltonSampleSource : public RuntimeSampleSource
{
public:
	inline HaltonSampleSource() = default;

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<HaltonSampleSource>)
	{
		ClassType clazz("halton");
		clazz.docName("Halton Sample Source");
		clazz.description(
			"Generating samples based on the Halton sequence. The samples "
			"generated are somewhat deterministic and can lead to visible "
			"patterns if the number of samples is too low.");
		clazz.baseOn<RuntimeSampleSource>();
		return clazz;
	}
};

}// end namespace ph
