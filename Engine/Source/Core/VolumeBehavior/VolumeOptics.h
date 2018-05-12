#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

namespace ph
{

class SurfaceHit;

class VolumeOptics
{
public:
	virtual ~VolumeOptics() = 0;

	virtual void evalPhaseFunction(
		const SurfaceHit& X, 
		const Vector3R&   I, 
		const Vector3R&   O, 
		real*             out_pf) const = 0;
};

}// end namespace ph