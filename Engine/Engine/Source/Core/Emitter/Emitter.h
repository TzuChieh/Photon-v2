#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Math/Transform/Transform.h"
#include "Math/Color/Spectrum.h"

#include <memory>

namespace ph
{

class Primitive;
class SurfaceHit;
class DirectEnergySampleQuery;
class Ray;
class Time;
class SampleFlow;

class Emitter
{
public:
	Emitter();
	virtual ~Emitter();

	virtual void evalEmittedRadiance(const SurfaceHit& X, math::Spectrum* out_radiance) const = 0;
	virtual void genDirectSample(DirectEnergySampleQuery& query, SampleFlow& sampleFlow) const = 0;

	// FIXME: ray time
	virtual void emitRay(SampleFlow& sampleFlow, Ray* out_ray, math::Spectrum* out_Le, math::Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const = 0;

	virtual real calcDirectSamplePdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const = 0;

	virtual real calcRadiantFluxApprox() const;
};

// In-header Implementations:

inline real Emitter::calcRadiantFluxApprox() const
{
	return 1.0_r;
}

}// end namespace ph