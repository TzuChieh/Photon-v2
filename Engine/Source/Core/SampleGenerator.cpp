#include "Core/SampleGenerator.h"

namespace ph
{

SampleGenerator::SampleGenerator(const uint32 sppBudget) : 
	m_sppBudget(sppBudget)
{

}

SampleGenerator::~SampleGenerator() = default;

}// end namespace ph