#include "EngineEnv/SampleSource/HaltonSampleSource.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "Core/SampleGenerator/SGHalton.h"

namespace ph
{

void HaltonSampleSource::cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked)
{
	cooked.addSampleGenerator(
		std::make_unique<SGHalton>(getNumSamples(), m_permutation, m_sequence));
}

}// end namespace ph
