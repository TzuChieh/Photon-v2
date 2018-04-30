#pragma once

#include "Frame/Operator/FrameOperator.h"
#include "Common/primitive_type.h"

namespace ph
{

class JRToneMapping final : public FrameOperator
{
public:
	JRToneMapping();
	virtual ~JRToneMapping() = 0;

	virtual void operate(const HdrRgbFrame& input, HdrRgbFrame& output) const = 0;

	void setExposure(real exposure);

private:
	real m_exposure;
};

}// end namespace ph