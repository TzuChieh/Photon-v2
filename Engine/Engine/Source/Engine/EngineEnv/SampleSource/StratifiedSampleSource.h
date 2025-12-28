#pragma once

#include "Engine/EngineEnv/SampleSource/RuntimeSampleSource.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

class StratifiedSampleSource : public RuntimeSampleSource
{
public:
	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override;

public:
	PH_DEFINE_SDL_CLASS(StratifiedSampleSource, clazz)
	{
		clazz.typeName("stratified");
		clazz.docName("Stratified Sample Source");
		clazz.description("Generating samples based on engine provided dimensional hints.");
		clazz.baseOn<RuntimeSampleSource>();
	}
};

}// end namespace ph
