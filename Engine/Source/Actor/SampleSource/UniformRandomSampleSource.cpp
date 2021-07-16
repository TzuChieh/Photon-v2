#include "Actor/SampleSource/UniformRandomSampleSource.h"
#include "Actor/CoreCookedUnit.h"
#include "Core/SampleGenerator/SGUniformRandom.h"

namespace ph
{

void UniformRandomSampleSource::cook(const CoreCookingContext& ctx, CoreCookedUnit& out_cooked)
{
	out_cooked.addSampleGenerator(
		std::make_unique<SGUniformRandom>(getNumSamples()));
}

}// end namespace ph
