#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Core/Texture/TSampler.h"
#include "Math/constant.h"
#include "Core/Texture/SampleLocation.h"
#include "Core/SampleGenerator/SampleFlow.h"
#include "Math/Color/color_spaces.h"

namespace ph
{

DiffuseSurfaceEmitter::DiffuseSurfaceEmitter(
	const Primitive* const surface,
	const std::shared_ptr<TTexture<math::Spectrum>>& emittedEnergy,
	const EmitterFeatureSet featureSet)

	: DiffuseSurfaceEmitterBase(featureSet)

	, m_surface(surface)
	, m_emittedEnergy(emittedEnergy)
{
	PH_ASSERT(surface);
	PH_ASSERT(emittedEnergy);
}

void DiffuseSurfaceEmitter::evalEmittedEnergy(const SurfaceHit& Xe, math::Spectrum* const out_energy) const
{
	evalEnergyFromSurface(getEmittedEnergy(), Xe, out_energy);
}

void DiffuseSurfaceEmitter::genDirectSample(
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	genDirectSampleFromSurface(getSurface(), getEmittedEnergy(), query, sampleFlow, probe);
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
	emitRayFromSurface(getSurface(), getEmittedEnergy(), query, sampleFlow, probe);
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
