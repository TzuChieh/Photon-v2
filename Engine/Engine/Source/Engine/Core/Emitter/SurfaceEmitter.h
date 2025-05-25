#pragma once

#include "Engine/Core/Emitter/Emitter.h"
#include "Engine/Math/math_fwd.h"
#include "Engine/Math/Transform/Transform.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Core/LTA/PDF.h"

#include <Common/primitive_type.h>

#include <memory>

namespace ph { class Primitive; }

namespace ph
{

class SurfaceEmitter : public Emitter
{
public:
	explicit SurfaceEmitter(EmitterFeatureSet featureSet = defaultFeatureSet);

	void evalEmittedEnergy(const SurfaceHit& Xe, math::Spectrum* out_energy) const override = 0;

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
	@return Can the emitter emit energy in `emitDir` given surface normal `N`. This method
	also considers front/back face emission settings.
	*/
	bool canEmit(const math::Vector3R& emitDir, const math::Vector3R& N) const;

	/*!
	Performs `calcDirectPdf()` on the source primitive specified by `query.inputs`. This computes
	solid angle domain PDF of sampling the surface emitter (as represented by the source primitive).
	@note
	- Handles `EEmitterFeatureSet::DirectSample`
	*/
	void calcDirectPdfWForSrcPrimitive(
		DirectEnergyPdfQuery& query,
		const lta::PDF& pickPdf = lta::PDF::D(1),
		const lta::PDF& emitPosUvwPdf = {}) const;

	bool m_isBackFaceEmission;
};

}// end namespace ph
