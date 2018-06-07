#include "Core/Integrator/Integrator.h"

namespace ph
{

Integrator::Integrator() = default;

Integrator::Integrator(const Integrator& other) = default;

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

Integrator& Integrator::operator = (const Integrator& rhs)
{
	m_statistics = rhs.m_statistics;
}

void swap(Integrator& first, Integrator& second)
{
	using std::swap;

	swap(first.m_statistics, second.m_statistics);
}

// command interface

Integrator::Integrator(const InputPacket& packet) :
	Integrator()
{}

SdlTypeInfo Integrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "integrator");
}

void Integrator::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph