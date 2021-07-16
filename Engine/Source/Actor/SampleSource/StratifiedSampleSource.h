#pragma once

#include "Actor/SampleSource/RuntimeSampleSource.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class StratifiedSampleSource : public RuntimeSampleSource
{
public:
	inline StratifiedSampleSource() = default;

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& out_cooked) override;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<StratifiedSampleSource>)
	{
		ClassType clazz("stratified");
		clazz.description("Generating samples based on engine provided dimensional hints.");
		clazz.baseOn<RuntimeSampleSource>();
		return clazz;
	}
};

}// end namespace ph
