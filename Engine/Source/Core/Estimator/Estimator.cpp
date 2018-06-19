#include "Core/Estimator/Estimator.h"

namespace ph
{

Estimator::Estimator() = default;

Estimator::~Estimator() = default;

// command interface

Estimator::Estimator(const InputPacket& packet) :
	Estimator()
{

}

SdlTypeInfo Estimator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ESTIMATOR, "estimator");
}

void Estimator::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph