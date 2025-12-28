#pragma once

#include "Engine/EngineEnv/SampleSource/RuntimeSampleSource.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

class UniformRandomSampleSource : public RuntimeSampleSource
{
public:
	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override;

public:
	PH_DEFINE_SDL_CLASS(UniformRandomSampleSource, clazz)
	{
		clazz.typeName("uniform-random");
		clazz.docName("Uniform Random Sample Source");
		clazz.description("Generating samples in a completely random fashion.");
		clazz.baseOn<RuntimeSampleSource>();
	}
};

}// end namespace ph
