#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Math/TVector3.h"
#include "Actor/Geometry/Geometry.h"
#include "Core/Texture/constant_textures.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/Intersection/Query/PrimitivePosPdfQuery.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Emitter/Query/DirectEnergyPdfQuery.h"
#include "Core/Emitter/Query/EnergyEmissionSampleQuery.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Core/Texture/TSampler.h"
#include "Math/constant.h"
#include "Math/Color/Spectrum.h"
#include "Core/Texture/SampleLocation.h"
#include "Math/Geometry/THemisphere.h"
#include "Math/TOrthonormalBasis3.h"
#include "Core/SampleGenerator/SampleFlow.h"
#include "Core/LTA/lta.h"
#include "Math/Color/color_spaces.h"

#include <limits>

namespace ph
{

DiffuseSurfaceEmitter::DiffuseSurfaceEmitter(
	const Primitive* const surface,
	const std::shared_ptr<TTexture<math::Spectrum>>& emittedEnergy,
	const EmitterFeatureSet featureSet)

	: SurfaceEmitter(featureSet)

	, m_surface(surface)
	, m_emittedEnergy(emittedEnergy)
{
	PH_ASSERT(surface);
	PH_ASSERT(emittedEnergy);
}

void DiffuseSurfaceEmitter::evalEmittedEnergy(const SurfaceHit& X, math::Spectrum* const out_energy) const
{
	const math::Vector3R emitDir = X.getIncidentRay().getDir().mul(-1.0_r);
	if(!canEmit(emitDir, X.getShadingNormal()))
	{
		out_energy->setColorValues(0.0_r);
		return;
	}

	// TODO: able to specify channel or restrict it
	TSampler<math::Spectrum> sampler(math::EColorUsage::EMR);
	*out_energy = sampler.sample(getEmittedEnergy(), X);
}

void DiffuseSurfaceEmitter::genDirectSample(
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
	getSurface().genPosSample(posSample, sampleFlow, probe);
	if(!posSample.outputs)
	{
		return;
	}

	HitDetail detail;
	HitProbe(probe).calcHitDetail(posSample.outputs.getObservationRay(), &detail);

	const auto emitterToTargetPos = query.inputs.getTargetPos() - posSample.outputs.getPos();
	if(!canEmit(emitterToTargetPos, detail.getShadingNormal()))
	{
		return;
	}

	// TODO: use sampler with surface hit
	math::Spectrum emittedEnergy;
	getEmittedEnergy().sample(
		SampleLocation(detail.getUVW(), math::EColorUsage::EMR), &emittedEnergy);

	query.outputs.setEmitPos(posSample.outputs.getPos());
	query.outputs.setEmittedEnergy(emittedEnergy);
	query.outputs.setSrcPrimitive(m_surface);
	query.outputs.setPdf(lta::PDF::W(lta::pdfA_to_pdfW(
		posSample.outputs.getPdfA(), emitterToTargetPos, detail.getShadingNormal())));
	query.outputs.setObservationRay(posSample.outputs.getObservationRay());
}

void DiffuseSurfaceEmitter::calcDirectPdf(DirectEnergyPdfQuery& query) const
{
	calcDirectPdfWForSrcPrimitive(query);
}

void DiffuseSurfaceEmitter::emitRay(
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
	getSurface().genPosSample(posSample, sampleFlow, probe);
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
	HitProbe(probe).calcHitDetail(posSample.outputs.getObservationRay(), &detail);

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
	probe.replaceBaseHitRayTWith(emittedRay.getMinT());

	query.outputs.setEmittedRay(emittedRay);
	query.outputs.setPdf(posSample.outputs.getPdfPos(), lta::PDF::W(pdfW));
	query.outputs.setEmittedEnergy(
		TSampler<math::Spectrum>(math::EColorUsage::EMR).sample(
			*m_emittedEnergy, detail.getUVW()));
}

real DiffuseSurfaceEmitter::calcRadiantFluxApprox() const
{
	// Randomly pick a point on the surface

	// TODO: more samples can be better
	// FIXME: use random time

	PrimitivePosSampleQuery posSample;
	posSample.inputs.set(Time{});
	SampleFlow randomFlow;
	HitProbe probe;
	getSurface().genPosSample(posSample, randomFlow, probe);
	if(!posSample.outputs)
	{
		return SurfaceEmitter::calcRadiantFluxApprox();
	}

	// FIXME: this assumption is too bad
	// And assume the surface emits constant radiance sampled from that point

	HitDetail detail;
	probe.calcHitDetail(posSample.outputs.getObservationRay(), &detail);

	const auto sampledL = TSampler<math::Spectrum>(math::EColorUsage::EMR).sample(
		*m_emittedEnergy, detail.getUVW());

	const real radiance     = math::estimate_color_energy<math::Spectrum::getColorSpace(), real>(sampledL.getColorValues());
	const real extendedArea = getSurface().calcExtendedArea();
	const real radiantFlux  = radiance * extendedArea * math::constant::pi<real>;

	return radiantFlux > 0.0_r ? radiantFlux : SurfaceEmitter::calcRadiantFluxApprox();
}

}// end namespace ph
