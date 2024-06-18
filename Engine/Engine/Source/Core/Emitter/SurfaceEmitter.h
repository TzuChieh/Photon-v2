#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Math/Transform/Transform.h"
#include "Math/Color/Spectrum.h"
#include "Core/Emitter/Emitter.h"
#include "Core/Texture/TTexture.h"
#include "Core/LTA/PDF.h"

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

	void calcDirectPdf(DirectEnergyPdfQuery& query) const override = 0;

	void emitRay(
		EnergyEmissionSampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override = 0;
	
	virtual void setFrontFaceEmit();
	virtual void setBackFaceEmit();

protected:
	/*!
	@return Can the emitter emit energy in `emitDirection` given surface normal `N`. This method
	also considers front/back face emission settings.
	*/
	bool canEmit(const math::Vector3R& emitDirection, const math::Vector3R& N) const;

	/*!
	Performs `calcDirectPdf()` on the source primitive specified by `query`. This computes solid
	angle domain PDF of sampling the emitter (as represented by the source primitive).
	*/
	void calcDirectPdfWForSrcPrimitive(
		DirectEnergyPdfQuery& query,
		const lta::PDF& pickPdf = lta::PDF::D(1),
		const lta::PDF& emitPosUvwPdf = {}) const;

	bool m_isBackFaceEmission;
};

}// end namespace ph
