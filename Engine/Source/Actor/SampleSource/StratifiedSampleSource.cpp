#include "Actor/SampleSource/StratifiedSampleSource.h"
#include "Actor/CoreCookedUnit.h"
#include "Core/SampleGenerator/SGStratified.h"

namespace ph
{

void StratifiedSampleSource::cook(const CoreCookingContext& ctx, CoreCookedUnit& out_cooked)
{
	out_cooked.addSampleGenerator(
		std::make_unique<SGStratified>(getNumSamples()));
}

}// end namespace ph
