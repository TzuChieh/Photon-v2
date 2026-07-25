#include "Engine/Core/Emitter/DiffuseSurfaceEmitterBase.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/Texture/constant_textures.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Engine/Core/Intersection/Query/PrimitivePosPdfQuery.h"
#include "Engine/Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Engine/Core/Emitter/Query/DirectEnergyPdfQuery.h"
#include "Engine/Core/Emitter/Query/EnergyEmissionSampleQuery.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/HitProbe.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Texture/TSampler.h"
#include "Engine/Math/constant.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Math/Geometry/THemisphere.h"
#include "Engine/Math/TOrthonormalBasis3.h"
#include "Engine/Core/SampleGenerator/SampleFlow.h"
#include "Engine/Core/LTA/lta.h"
#include "Engine/Math/Color/color_spaces.h"

#include <limits>

namespace ph
{

DiffuseSurfaceEmitterBase::DiffuseSurfaceEmitterBase(
	const EmitterFeatureSet featureSet)

	: SurfaceEmitter(featureSet)
{}

void DiffuseSurfaceEmitterBase::evalEnergyFromSurface(
	const TTexture<math::Spectrum>& energy,
	const SurfaceHit& Xe,
	math::Spectrum* const out_energy) const
{
	const math::Vector3R emitDir = Xe.getIncidentRay().getDir().mul(-1.0_r);
	if(!canEmit(emitDir, Xe.getShadingNormal()))
	{
		out_energy->setColorValues(0.0_r);
		return;
	}

	// TODO: able to specify channel or restrict it
	TSampler<math::Spectrum> sampler(math::EColorUsage::EMR);
	*out_energy = sampler.sample(energy, Xe);
}

void DiffuseSurfaceEmitterBase::genDirectSampleFromSurface(
	const Primitive& surface,
	const TTexture<math::Spectrum>& energy,
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	if(getFeatureSet().hasNo(EEmitterFeatureSet::DirectSample))
	{
		return;
	}

	PrimitivePosSampleQuery posSample;
	posSample.inputs.set(query.inputs);
	surface.genPosSample(posSample, sampleFlow, probe);
	if(!posSample.outputs)
	{
		return;
	}

	const SurfaceHit Xe(posSample.outputs.getObservationRay(), probe, ESurfaceHitReason::SampledPos);
	const auto emitterToTargetPos = query.inputs.getTargetPos() - posSample.outputs.getPos();
	if(!canEmit(emitterToTargetPos, Xe.getShadingNormal()))
	{
		return;
	}

	const TSampler<math::Spectrum> sampler(math::EColorUsage::EMR);
	const math::Spectrum emittedEnergy = sampler.sample(energy, Xe);

	query.outputs.setEmitPos(posSample.outputs.getPos());
	query.outputs.setEmittedEnergy(emittedEnergy);
	query.outputs.setSrcPrimitive(&surface);
	query.outputs.setPdf(lta::PDF::W(lta::pdfA_to_pdfW(
		posSample.outputs.getPdfA(), emitterToTargetPos, Xe.getShadingNormal())));
	query.outputs.setObservationRay(posSample.outputs.getObservationRay());
}

void DiffuseSurfaceEmitterBase::emitRayFromSurface(
	const Primitive& surface,
	const TTexture<math::Spectrum>& energy,
	EnergyEmissionSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	if(getFeatureSet().hasNo(EEmitterFeatureSet::EmissionSample))
	{
		return;
	}

	PrimitivePosSampleQuery posSample;
	posSample.inputs.set(query.inputs.getTime());
	surface.genPosSample(posSample, sampleFlow, probe);
	if(!posSample.outputs)
	{
		return;
	}

	real pdfW;
	auto localRayDir = math::THemisphere<real>::makeUnit().sampleToSurfaceCosThetaWeighted(
		sampleFlow.flow2D(), &pdfW);
	if(m_isBackFaceEmission)
	{
		localRayDir.negateLocal();
	}

	HitDetail detail;
	probe.calcHitDetail(posSample.outputs.getObservationRay(), &detail);

	const auto sampleBasis = math::Basis3R::makeFromUnitY(detail.getShadingNormal());
	const auto rayDir = sampleBasis.localToWorld(localRayDir);
	if(!canEmit(rayDir, detail.getShadingNormal()))
	{
		return;
	}

	const Ray emittedRay(
		posSample.outputs.getPos(),
		rayDir.normalize(),
		0,
		std::numeric_limits<real>::max(),
		query.inputs.getTime());

	// The emitted ray will be representing the hit event
	probe.replaceBaseHitRayT(emittedRay.getMinT());

	const SurfaceHit Xe(emittedRay, probe, ESurfaceHitReason::SampledPosDir);
	const TSampler<math::Spectrum> sampler(math::EColorUsage::EMR);
	const math::Spectrum emittedEnergy = sampler.sample(energy, Xe);

	query.outputs.setEmittedRay(emittedRay);
	query.outputs.setPdf(posSample.outputs.getPdfPos(), lta::PDF::W(pdfW));
	query.outputs.setEmittedEnergy(emittedEnergy);
}

}// end namespace ph
