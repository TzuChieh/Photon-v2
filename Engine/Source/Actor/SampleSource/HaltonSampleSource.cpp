#include "Actor/SampleSource/HaltonSampleSource.h"
#include "Actor/CoreCookedUnit.h"
#include "Core/SampleGenerator/SGHalton.h"

namespace ph
{

void HaltonSampleSource::cook(const CoreCookingContext& ctx, CoreCookedUnit& out_cooked)
{
	out_cooked.addSampleGenerator(
		std::make_unique<SGHalton>(getNumSamples()));
}

}// end namespace ph
