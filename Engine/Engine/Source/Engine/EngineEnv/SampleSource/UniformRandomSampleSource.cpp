#include "Engine/EngineEnv/SampleSource/UniformRandomSampleSource.h"
#include "Engine/EngineEnv/CoreCookedUnit.h"
#include "Engine/Core/SampleGenerator/SGUniformRandom.h"

namespace ph
{

void UniformRandomSampleSource::cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked)
{
	cooked.addSampleGenerator(
		std::make_unique<SGUniformRandom>(getNumSamples()));
}

}// end namespace ph
