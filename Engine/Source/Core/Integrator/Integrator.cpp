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

void Integrator::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph