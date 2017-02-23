#include "Core/SampleGenerator/SampleGenerator.h"
#include "FileIO/InputPacket.h"

namespace ph
{

SampleGenerator::SampleGenerator(const uint32 sppBudget) : 
	m_sppBudget(sppBudget)
{

}

SampleGenerator::SampleGenerator(const InputPacket& packet)
{
	m_sppBudget = packet.getInteger("spp-budget", 0, DataTreatment::REQUIRED());
}

SampleGenerator::~SampleGenerator() = default;

}// end namespace ph