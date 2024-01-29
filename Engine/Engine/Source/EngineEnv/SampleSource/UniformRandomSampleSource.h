#pragma once

#include "EngineEnv/SampleSource/RuntimeSampleSource.h"
#include "SDL/sdl_interface.h"

namespace ph
{

class UniformRandomSampleSource : public RuntimeSampleSource
{
public:
	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<UniformRandomSampleSource>)
	{
		ClassType clazz("uniform-random");
		clazz.docName("Uniform Random Sample Source");
		clazz.description("Generating samples in a completely random fashion.");
		clazz.baseOn<RuntimeSampleSource>();
		return clazz;
	}
};

}// end namespace ph
