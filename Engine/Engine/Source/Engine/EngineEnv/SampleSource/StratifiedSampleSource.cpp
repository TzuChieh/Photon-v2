#include "Engine/EngineEnv/SampleSource/StratifiedSampleSource.h"
#include "Engine/EngineEnv/CoreCookedUnit.h"
#include "Engine/Core/SampleGenerator/SGStratified.h"

namespace ph
{

void StratifiedSampleSource::cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked)
{
	cooked.addSampleGenerator(
		std::make_unique<SGStratified>(getNumSamples()));
}

}// end namespace ph
