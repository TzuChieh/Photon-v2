#include "Engine/EngineEnv/SampleSource/HaltonSampleSource.h"
#include "Engine/EngineEnv/CoreCookedUnit.h"
#include "Engine/Core/SampleGenerator/SGHalton.h"

namespace ph
{

void HaltonSampleSource::cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked)
{
	cooked.addSampleGenerator(
		std::make_unique<SGHalton>(getNumSamples(), m_permutation, m_sequence));
}

}// end namespace ph
