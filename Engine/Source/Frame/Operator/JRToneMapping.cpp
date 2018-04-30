#include "Frame/Operator/JRToneMapping.h"
#include "Common/assertion.h"

namespace ph
{

JRToneMapping::JRToneMapping() : 
	FrameOperator(),
	m_exposure(1.0_r)
{}

JRToneMapping::~JRToneMapping() = default;

void JRToneMapping::operate(const HdrRgbFrame& input, HdrRgbFrame& output) const
{
	// TODO
}

void JRToneMapping::setExposure(const real exposure)
{
	PH_ASSERT(exposure > 0.0_r);

	m_exposure = exposure;
}

}// end namespace ph