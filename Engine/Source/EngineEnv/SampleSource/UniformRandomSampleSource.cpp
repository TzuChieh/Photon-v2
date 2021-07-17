#include "EngineEnv/SampleSource/UniformRandomSampleSource.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "Core/SampleGenerator/SGUniformRandom.h"

namespace ph
{

void UniformRandomSampleSource::cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked)
{
	cooked.addSampleGenerator(
		std::make_unique<SGUniformRandom>(getNumSamples()));
}

}// end namespace ph
