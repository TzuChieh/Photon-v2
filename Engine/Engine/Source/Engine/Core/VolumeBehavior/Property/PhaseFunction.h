#pragma once

#include "Engine/Math/math_fwd.h"

#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

class SurfaceHit;

class PhaseFunction
{
public:
	virtual ~PhaseFunction();

	// FIXME: use volume hit
	virtual void evalPhaseFunc(
		const SurfaceHit&     X,
		const math::Vector3R& I,
		const math::Vector3R& O,
		real*                 out_pf) const = 0;
};

}// end namespace ph
