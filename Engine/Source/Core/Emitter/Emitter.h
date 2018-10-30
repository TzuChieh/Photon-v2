#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Math/Transform/Transform.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>

namespace ph
{

class Primitive;
class SurfaceHit;
class DirectLightSample;
class Ray;
class Time;

class Emitter
{
public:
	Emitter();
	virtual ~Emitter();

	virtual void evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* out_radiance) const = 0;
	virtual void genDirectSample(DirectLightSample& sample) const = 0;

	// FIXME: ray time
	virtual void genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const = 0;

	virtual real calcDirectSamplePdfW(const SurfaceHit& emitPos, const Vector3R& targetPos) const = 0;

	// HACK
	virtual bool isBackground() const
	{
		return false;
	}

	virtual real calcRadiantFluxApprox() const;
};

// In-header Implementations:

inline real Emitter::calcRadiantFluxApprox() const
{
	return 1.0_r;
}

}// end namespace ph