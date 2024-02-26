#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Math/Transform/Transform.h"
#include "Math/Color/Spectrum.h"
#include "Core/Emitter/Emitter.h"
#include "Core/Texture/TTexture.h"

#include <memory>

namespace ph { class Primitive; }

namespace ph
{

class SurfaceEmitter : public Emitter
{
public:
	SurfaceEmitter();

	void evalEmittedRadiance(const SurfaceHit& X, math::Spectrum* out_radiance) const override = 0;

	void genDirectSample(
		DirectEnergySampleQuery& query, 
		SampleFlow& sampleFlow,
		HitProbe& probe) const override = 0;

	void calcDirectSamplePdfW(
		DirectEnergySamplePdfQuery& query,
		HitProbe& probe) const override = 0;

	void emitRay(
		EnergyEmissionSampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override = 0;
	
	virtual void setFrontFaceEmit();
	virtual void setBackFaceEmit();

protected:
	bool canEmit(const math::Vector3R& emitDirection, const math::Vector3R& N) const;

	void calcDirectSamplePdfWForSingleSurface(
		DirectEnergySamplePdfQuery& query,
		HitProbe& probe) const;

	bool m_isBackFaceEmission;
};

}// end namespace ph
