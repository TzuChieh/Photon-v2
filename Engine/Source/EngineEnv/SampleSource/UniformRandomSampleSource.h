#pragma once

#include "EngineEnv/SampleSource/RuntimeSampleSource.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class UniformRandomSampleSource : public RuntimeSampleSource
{
public:
	inline UniformRandomSampleSource() = default;

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<UniformRandomSampleSource>)
	{
		ClassType clazz("uniform-random");
		clazz.description("Generating samples in a completely random fashion.");
		clazz.baseOn<RuntimeSampleSource>();
		return clazz;
	}
};

}// end namespace ph
