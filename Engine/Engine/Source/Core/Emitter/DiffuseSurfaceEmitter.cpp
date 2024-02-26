#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Math/TVector3.h"
#include "Actor/Geometry/Geometry.h"
#include "Core/Texture/constant_textures.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/Intersection/Query/PrimitivePosSamplePdfQuery.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Emitter/Query/DirectEnergySamplePdfQuery.h"
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
#include "Math/Color/spectral_samples.h"
#include "Math/Color/color_spaces.h"

#include <Common/assertion.h>

#include <limits>

namespace ph
{

DiffuseSurfaceEmitter::DiffuseSurfaceEmitter(const Primitive* const surface) :
	SurfaceEmitter(), 
	m_surface(surface),
	m_emittedRadiance(nullptr)
{
	PH_ASSERT(surface);

	const real extendedArea = surface->calcExtendedArea();
	m_reciExtendedArea = extendedArea > 0.0_r ? 1.0_r / extendedArea : 0.0_r;

	static const auto DEFAULT_RADIANCE = math::Spectrum().setSpectral(
		math::resample_illuminant_D65<math::ColorValue>(), math::EColorUsage::EMR);

	setEmittedRadiance(std::make_shared<TConstantTexture<math::Spectrum>>(DEFAULT_RADIANCE));
}

void DiffuseSurfaceEmitter::evalEmittedRadiance(const SurfaceHit& X, math::Spectrum* const out_radiance) const
{
	const math::Vector3R emitDir = X.getIncidentRay().getDirection().mul(-1.0_r);
	if(!canEmit(emitDir, X.getShadingNormal()))
	{
		out_radiance->setColorValues(0.0_r);
		return;
	}

	// TODO: able to specify channel or restrict it
	TSampler<math::Spectrum> sampler(math::EColorUsage::EMR);
	*out_radiance = sampler.sample(getEmittedRadiance(), X);
}

void DiffuseSurfaceEmitter::genDirectSample(
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	PrimitivePosSampleQuery posSample;
	posSample.inputs.set(query.inputs.getTime(), query.inputs.getTargetPos());

	m_surface->genPosSample(posSample, sampleFlow, probe);
	if(!posSample.outputs)
	{
		query.outputs.invalidate();
		return;
	}

	HitDetail detail;
	HitProbe(probe).calcHitDetail(posSample.outputs.getObservationRay(), &detail);

	const auto emitterToTargetPos = query.inputs.getTargetPos() - posSample.outputs.getPos();
	if(!canEmit(emitterToTargetPos, detail.getShadingNormal()))
	{
		query.outputs.invalidate();
		return;
	}

	// TODO: use sampler with surface hit
	math::Spectrum emittedEnergy;
	getEmittedRadiance().sample(
		SampleLocation(detail.getUVW(), math::EColorUsage::EMR), &emittedEnergy);

	query.outputs.setEmitPos(posSample.outputs.getPos());
	query.outputs.setEmittedEnergy(emittedEnergy);
	query.outputs.setSrcPrimitive(m_surface);
	query.outputs.setPdfW(lta::pdfA_to_pdfW(
		posSample.outputs.getPdfA(), emitterToTargetPos, detail.getShadingNormal()));
	query.outputs.setObservationRay(posSample.outputs.getObservationRay());
}

void DiffuseSurfaceEmitter::calcDirectSamplePdfW(
	DirectEnergySamplePdfQuery& query,
	HitProbe& probe) const
{
	calcDirectSamplePdfWForSingleSurface(query, probe);
}

void DiffuseSurfaceEmitter::emitRay(
	EnergyEmissionSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	PrimitivePosSampleQuery posSample;
	posSample.inputs.set(query.inputs.getTime());
	m_surface->genPosSample(posSample, sampleFlow, probe);
	if(!posSample.outputs)
	{
		query.outputs.invalidate();
		return;
	}

	real pdfW;
	const auto localRayDir = math::THemisphere<real>::makeUnit().sampleToSurfaceCosThetaWeighted(
		sampleFlow.flow2D(), &pdfW);

	HitDetail detail;
	HitProbe(probe).calcHitDetail(posSample.outputs.getObservationRay(), &detail);

	const auto sampleBasis = math::Basis3R::makeFromUnitY(detail.getShadingNormal());
	const auto rayDir = sampleBasis.localToWorld(localRayDir);

	const Ray emittedRay(
		posSample.outputs.getPos(),
		rayDir.normalize(),
		0,
		std::numeric_limits<real>::max(),
		query.inputs.getTime());

	// The emitted ray will be representing the hit event
	probe.replaceBaseHitRayTWith(emittedRay.getMinT());

	query.outputs.setEmittedRay(emittedRay);
	query.outputs.setPdf(posSample.outputs.getPdfA(), pdfW);
	query.outputs.setEmittedEnergy(
		TSampler<math::Spectrum>(math::EColorUsage::EMR).sample(
			*m_emittedRadiance, detail.getUVW()));
}

const Primitive* DiffuseSurfaceEmitter::getSurface() const
{
	return m_surface;
}

void DiffuseSurfaceEmitter::setEmittedRadiance(const std::shared_ptr<TTexture<math::Spectrum>>& emittedRadiance)
{
	PH_ASSERT(emittedRadiance);

	m_emittedRadiance = emittedRadiance;
}

const TTexture<math::Spectrum>& DiffuseSurfaceEmitter::getEmittedRadiance() const
{
	PH_ASSERT(m_emittedRadiance);

	return *m_emittedRadiance;
}

real DiffuseSurfaceEmitter::calcRadiantFluxApprox() const
{
	PH_ASSERT(m_emittedRadiance && m_surface);

	// Randomly pick a point on the surface

	// TODO: more samples can be better
	// FIXME: use random time

	PrimitivePosSampleQuery posSample;
	posSample.inputs.set(Time{});
	SampleFlow randomFlow;
	HitProbe probe;
	m_surface->genPosSample(posSample, randomFlow, probe);
	if(!posSample.outputs)
	{
		return SurfaceEmitter::calcRadiantFluxApprox();
	}

	// FIXME: this assumption is too bad

	// And assume the surface emits constant radiance sampled from that point

	HitDetail detail;
	probe.calcHitDetail(posSample.outputs.getObservationRay(), &detail);

	const auto sampledL = TSampler<math::Spectrum>(math::EColorUsage::EMR).sample(
		*m_emittedRadiance, detail.getUVW());

	const real radiance     = math::estimate_color_energy<math::Spectrum::getColorSpace(), real>(sampledL.getColorValues());
	const real extendedArea = m_surface->calcExtendedArea();
	const real radiantFlux  = radiance * extendedArea * math::constant::pi<real>;

	return radiantFlux > 0.0_r ? radiantFlux : SurfaceEmitter::calcRadiantFluxApprox();
}

}// end namespace ph
