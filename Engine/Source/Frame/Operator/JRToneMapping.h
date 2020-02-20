#pragma once

#include "Frame/Operator/FrameOperator.h"
#include "Common/primitive_type.h"
#include "Math/Geometry/TAABB2D.h"

namespace ph
{

/*
	An optimized formula proposed by Jim Hejl and Richard Burgess-Dawson 
	in their GDC talk. 
	
	This operator does not need gamma correction afterwards.

	Reference: http://filmicworlds.com/blog/filmic-tonemapping-operators/
*/
class JRToneMapping : public FrameOperator
{
public:
	template<typename T>
	using TAABB2D = math::TAABB2D<T>;

	JRToneMapping();

	void operate(HdrRgbFrame& frame) const override;

	void operateLocal(HdrRgbFrame& frame, const TAABB2D<uint32>& region) const;
	void setExposure(real exposure);

private:
	real m_exposure;
};

}// end namespace ph
