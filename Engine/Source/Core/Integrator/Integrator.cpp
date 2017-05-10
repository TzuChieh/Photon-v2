#include "Core/Integrator/Integrator.h"

namespace ph
{

Integrator::Integrator() = default;

Integrator::~Integrator() = default;

// command interface

Integrator::Integrator(const InputPacket& packet) :
	Integrator()
{

}

SdlTypeInfo Integrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "integrator");
}

ExitStatus Integrator::ciExecute(const std::shared_ptr<Integrator>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph