#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <memory>

namespace ph
{

class SurfaceHit;

class ScatterFunction
{
public:
	virtual ~ScatterFunction() = 0;

	// FIXME: use volume hit
	virtual void evalPhaseFunc(
		const SurfaceHit&     X,
		const math::Vector3R& I,
		const math::Vector3R& O,
		real*                 out_pf) const = 0;
};

}// end namespace ph
