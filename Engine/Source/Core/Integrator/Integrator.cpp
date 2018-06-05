#include "Core/Integrator/Integrator.h"

namespace ph
{

Integrator::Integrator() = default;

Integrator::~Integrator() = default;

Statistics::Record Integrator::asyncGetStatistics() const
{
	return m_statistics.record();
}

void Integrator::updateStatistics(const Statistics::Record& statistics)
{
	m_statistics.setNumMsElapsed(statistics.numMsElapsed);
	m_statistics.setNumSamplesTaken(statistics.numSamplesTaken);
	m_statistics.setTotalWork(statistics.totalWork);
	m_statistics.setWorkDone(statistics.workDone);
}

// command interface

Integrator::Integrator(const InputPacket& packet) :
	Integrator()
{

}

SdlTypeInfo Integrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "integrator");
}

void Integrator::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph