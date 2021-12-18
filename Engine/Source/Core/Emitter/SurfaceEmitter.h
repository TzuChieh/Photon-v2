#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Math/Transform/Transform.h"
#include "Math/Color/Spectrum.h"
#include "Core/Emitter/Emitter.h"
#include "Core/Texture/TTexture.h"
#include "Common/assertion.h"

#include <memory>

namespace ph
{

class Primitive;
class SurfaceHit;
class Ray;
class Time;

class SurfaceEmitter : public Emitter
{
public:
	SurfaceEmitter();

	void evalEmittedRadiance(const SurfaceHit& X, math::Spectrum* out_radiance) const override = 0;
	void genDirectSample(DirectEnergySampleQuery& query, SampleFlow& sampleFlow) const override = 0;

	// FIXME: ray time
	void emitRay(SampleFlow& sampleFlow, Ray* out_ray, math::Spectrum* out_Le, math::Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const override = 0;

	real calcDirectSamplePdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const override = 0;
	
	virtual void setFrontFaceEmit();
	virtual void setBackFaceEmit();

protected:
	bool m_isBackFaceEmission;

	bool canEmit(const math::Vector3R& emitDirection, const math::Vector3R& N) const;
	real calcPdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const;
	real calcPdfW(
		const Primitive* emitSurface, 
		const math::Vector3R& emitPos,
		const math::Vector3R& emitNormal,
		const math::Vector3R& targetPos) const;
};

}// end namespace ph
