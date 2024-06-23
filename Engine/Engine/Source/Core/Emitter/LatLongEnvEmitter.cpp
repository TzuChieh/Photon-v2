#include "Core/Emitter/LatLongEnvEmitter.h"
#include "Core/SurfaceHit.h"
#include "Core/Texture/TSampler.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Emitter/Query/DirectEnergyPdfQuery.h"
#include "Core/Emitter/Query/EnergyEmissionSampleQuery.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/Intersection/Query/PrimitivePosPdfQuery.h"
#include "Core/Intersection/Primitive.h"
#include "Core/LTA/lta.h"
#include "Math/constant.h"
#include "Math/math.h"
#include "Math/Geometry/TDisk.h"
#include "Math/TOrthonormalBasis3.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/profiling.h>

#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(LatLongEnvEmitter, Emitter);

LatLongEnvEmitter::LatLongEnvEmitter(
	const Primitive* const         envSurface,
	const RadianceTexture&         radiance,
	const math::Vector2S&          resolution,
	const EmitterFeatureSet        featureSet)

	: SurfaceEmitter(featureSet)

	, m_envSurface        (envSurface)
	, m_radiance          (radiance)
	, m_sampleDistribution()
	, m_radiantFluxApprox (0)
{
	PH_PROFILE_SCOPE();
	PH_ASSERT(envSurface);
	PH_ASSERT(radiance);
	PH_ASSERT_GT(resolution.x() * resolution.y(), 0);

	PH_LOG(LatLongEnvEmitter, Note,
		"constructing sample distribution with resolution {}", resolution.toString());

	constexpr auto USAGE = math::EColorUsage::EMR;
	const real rcpResolutionY = 1.0_r / static_cast<real>(resolution.y());
	const TSampler<math::Spectrum> sampler(USAGE);

	std::vector<real> sampleWeights(resolution.x() * resolution.y());
	for(std::size_t y = 0; y < resolution.y(); ++y)
	{
		const std::size_t baseIndex = y * resolution.x();
		const real        v         = (static_cast<real>(y) + 0.5_r) * rcpResolutionY;
		const real        sinTheta  = std::sin((1.0_r - v) * math::constant::pi<real>);

		for(std::size_t x = 0; x < resolution.x(); ++x)
		{
			const real           u        = (static_cast<real>(x) + 0.5_r) / static_cast<real>(resolution.x());
			const math::Spectrum sampledL = sampler.sample(*radiance, {u, v});

			// For non-nearest filtered textures, sample weights can be 0 while
			// there is still energy around that point (because its neighbor 
			// may have non-zero energy), ensure a lower bound to avoid this
			constexpr real MIN_LUMINANCE = 1e-6_r;
			const real luminance = std::max(sampledL.relativeLuminance(USAGE), MIN_LUMINANCE);

			// FIXME: using different PDF resolution can under sample the texture
			// use mipmaps perhaps?
			sampleWeights[baseIndex + x] = luminance * sinTheta;

			m_radiantFluxApprox += sampleWeights[baseIndex + x];
		}
	}

	m_sampleDistribution = math::TPwcDistribution2D<real>(sampleWeights.data(), resolution);
	m_radiantFluxApprox  = m_radiantFluxApprox * m_envSurface->calcExtendedArea();
}

void LatLongEnvEmitter::evalEmittedEnergy(
	const SurfaceHit& X, 
	math::Spectrum* const out_energy) const
{
	PH_ASSERT(out_energy);
	PH_ASSERT(m_radiance);

	TSampler<math::Spectrum> sampler(math::EColorUsage::EMR);
	*out_energy = sampler.sample(*m_radiance, X);
}

void LatLongEnvEmitter::genDirectSample(
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	if(getFeatureSet().hasNo(EEmitterFeatureSet::DirectSample))
	{
		return;
	}

	real uvSamplePdf;
	const math::Vector2R uvSample = m_sampleDistribution.sampleContinuous(
		sampleFlow.flow2D(), &uvSamplePdf);

	PrimitivePosSampleQuery posSample;
	posSample.inputs.set(
		query.inputs,
		{uvSample.x(), uvSample.y(), 0},
		{.value = uvSamplePdf, .domain = lta::EDomain::UV01});
	m_envSurface->genPosSample(posSample, sampleFlow, probe);
	if(!posSample.outputs)
	{
		return;
	}

	HitDetail detail;
	HitProbe(probe).calcHitDetail(posSample.outputs.getObservationRay(), &detail);

	// TODO: use sampler with surface hit
	query.outputs.setEmittedEnergy(TSampler<math::Spectrum>{math::EColorUsage::EMR}.sample(
		*m_radiance, uvSample));
	query.outputs.setEmitPos(posSample.outputs.getPos());
	query.outputs.setPdf(lta::PDF::W(lta::pdfA_to_pdfW(
		posSample.outputs.getPdfA(), 
		query.inputs.getTargetPos() - posSample.outputs.getPos(),
		detail.getShadingNormal())));
	query.outputs.setSrcPrimitive(m_envSurface);
	query.outputs.setObservationRay(posSample.outputs.getObservationRay());
}

void LatLongEnvEmitter::calcDirectPdf(DirectEnergyPdfQuery& query) const
{
	if(getFeatureSet().hasNo(EEmitterFeatureSet::DirectSample))
	{
		return;
	}

	const math::Vector3R uvw(query.inputs.getXe().getDetail().getUVW());
	const real latLong01Pdf = m_sampleDistribution.pdfContinuous({uvw.x(), uvw.y()});

	PrimitivePosPdfQuery posPdf;
	posPdf.inputs.set(query.inputs, {.value = latLong01Pdf, .domain = lta::EDomain::UV01});
	m_envSurface->calcPosPdf(posPdf);
	if(!posPdf.outputs)
	{
		return;
	}

	query.outputs.setPdf(lta::PDF::W(lta::pdfA_to_pdfW(
		posPdf.outputs.getPdfA(),
		query.inputs.getTargetPos() - query.inputs.getEmitPos(),
		query.inputs.getXe().getShadingNormal())));
}

void LatLongEnvEmitter::emitRay(
	EnergyEmissionSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	if(getFeatureSet().hasNo(EEmitterFeatureSet::EmissionSample))
	{
		return;
	}

	real uvSamplePdf;
	const math::Vector2R uvSample = m_sampleDistribution.sampleContinuous(
		sampleFlow.flow2D(), &uvSamplePdf);

	PrimitivePosSampleQuery posSample;
	posSample.inputs.set(
		query.inputs.getTime(),
		std::nullopt,
		{uvSample.x(), uvSample.y(), 0},
		{.value = uvSamplePdf, .domain = lta::EDomain::UV01},
		true);
	m_envSurface->genPosSample(posSample, sampleFlow, probe);
	if(!posSample.outputs)
	{
		return;
	}

	// This emitter needs the suggested sample direction
	PH_ASSERT(!posSample.outputs.getPdfDir().isEmpty());
	PH_ASSERT_IN_RANGE(posSample.outputs.getDir().lengthSquared(), 0.9_r, 1.1_r);

	const auto emittedEnergy = TSampler<math::Spectrum>{math::EColorUsage::EMR}.sample(
		*m_radiance, uvSample);

	auto emittedRay = posSample.outputs.getObservationRay();
	emittedRay.setRange(0, std::numeric_limits<real>::max());

	query.outputs.setEmittedEnergy(emittedEnergy);
	query.outputs.setEmittedRay(emittedRay);
	query.outputs.setPdf(posSample.outputs.getPdfPos(), posSample.outputs.getPdfDir());

	probe.replaceBaseHitRayTWith(emittedRay.getMinT());
}

real LatLongEnvEmitter::calcRadiantFluxApprox() const
{
	PH_ASSERT(m_envSurface && m_radiance);

	return m_radiantFluxApprox;
}

void LatLongEnvEmitter::setFrontFaceEmit()
{
	PH_LOG(LatLongEnvEmitter, Debug,
		"Setting front/back face emit has no effect on this emitter (as it is assumed to encompass "
		"the whole scene).");
}

void LatLongEnvEmitter::setBackFaceEmit()
{
	PH_LOG(LatLongEnvEmitter, Debug,
		"Setting front/back face emit has no effect on this emitter (as it is assumed to encompass "
		"the whole scene).");
}

}// end namespace ph
