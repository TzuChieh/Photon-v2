#pragma once

#include "EngineEnv/SampleSource/RuntimeSampleSource.h"
#include "SDL/sdl_interface.h"

namespace ph
{

class StratifiedSampleSource : public RuntimeSampleSource
{
public:
	inline StratifiedSampleSource() = default;

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<StratifiedSampleSource>)
	{
		ClassType clazz("stratified");
		clazz.docName("Stratified Sample Source");
		clazz.description("Generating samples based on engine provided dimensional hints.");
		clazz.baseOn<RuntimeSampleSource>();
		return clazz;
	}
};

}// end namespace ph
