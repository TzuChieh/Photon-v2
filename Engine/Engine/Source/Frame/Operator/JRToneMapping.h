#pragma once

#include "Frame/Operator/FrameOperator.h"
#include "Math/Geometry/TAABB2D.h"

#include <Common/primitive_type.h>

namespace ph
{

/*! @brief Maps HDR values to LDR.

Uses an optimized formula proposed by Jim Hejl and Richard Burgess-Dawson 
in their GDC talk. This operator does not need gamma correction afterwards.

Reference: http://filmicworlds.com/blog/filmic-tonemapping-operators/
*/
class JRToneMapping : public FrameOperator
{
public:
	template<typename T>
	using TAABB2D = math::TAABB2D<T>;

	JRToneMapping();

	void operate(const HdrRgbFrame& srcFrame, HdrRgbFrame* out_dstFrame) override;

	void operateLocal(HdrRgbFrame& frame, const TAABB2D<uint32>& region) const;
	void setExposure(real exposure);

private:
	real m_exposure;
};

}// end namespace ph
