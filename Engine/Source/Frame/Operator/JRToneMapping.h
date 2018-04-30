#pragma once

#include "Frame/Operator/FrameOperator.h"
#include "Common/primitive_type.h"

namespace ph
{

/*
	An optimized formula proposed by Jim Hejl and Richard Burgess-Dawson 
	in their GDC talk. 
	
	This operator does not need gamma correction afterwards.
*/
class JRToneMapping final : public FrameOperator
{
public:
	JRToneMapping();
	virtual ~JRToneMapping() override;

	virtual void operate(HdrRgbFrame& frame) const override;

	void setExposure(real exposure);

private:
	real m_exposure;
};

}// end namespace ph