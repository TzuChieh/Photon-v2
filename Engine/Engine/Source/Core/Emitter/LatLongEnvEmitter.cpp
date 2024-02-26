#include "Core/Emitter/LatLongEnvEmitter.h"
#include "Core/SurfaceHit.h"
#include "Core/Texture/TSampler.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Emitter/Query/DirectEnergySamplePdfQuery.h"
#include "Core/Emitter/Query/EnergyEmissionSampleQuery.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/Intersection/Query/PrimitivePosSamplePdfQuery.h"
#include "Core/Intersection/PLatLongEnvSphere.h"
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
	const PLatLongEnvSphere* const surface,
	const RadianceTexture&         radiance,
	const math::Vector2S&          resolution) :

	m_surface           (surface),
	m_radiance          (radiance),
	m_sampleDistribution(),
	m_radiantFluxApprox (0)
{
	PH_PROFILE_SCOPE();
	PH_ASSERT(surface);
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
	m_radiantFluxApprox  = m_radiantFluxApprox * m_surface->calcExtendedArea();
}

void LatLongEnvEmitter::evalEmittedRadiance(
	const SurfaceHit& X, 
	math::Spectrum* const out_radiance) const
{
	PH_ASSERT(out_radiance);
	PH_ASSERT(m_radiance);

	TSampler<math::Spectrum> sampler(math::EColorUsage::EMR);
	*out_radiance = sampler.sample(*m_radiance, X);
}

void LatLongEnvEmitter::genDirectSample(
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	real uvSamplePdf;
	const math::Vector2R uvSample = m_sampleDistribution.sampleContinuous(
		sampleFlow.flow2D(), &uvSamplePdf);

	PrimitivePosSampleQuery posSample;
	posSample.inputs.set(query.inputs.getTime(), query.inputs.getTargetPos());

	m_surface->genPosSampleWithObservationPos(uvSample, uvSamplePdf, posSample, probe);
	if(!posSample.outputs)
	{
		query.outputs.invalidate();
		return;
	}

	HitDetail detail;
	HitProbe(probe).calcHitDetail(posSample.outputs.getObservationRay(), &detail);

	// TODO: use sampler with surface hit
	query.outputs.setEmittedEnergy(TSampler<math::Spectrum>{math::EColorUsage::EMR}.sample(
		*m_radiance, uvSample));
	query.outputs.setEmitPos(posSample.outputs.getPos());
	query.outputs.setPdfW(lta::pdfA_to_pdfW(
		posSample.outputs.getPdfA(), 
		query.inputs.getTargetPos() - posSample.outputs.getPos(),
		detail.getShadingNormal()));
	query.outputs.setSrcPrimitive(m_surface);
	query.outputs.setObservationRay(posSample.outputs.getObservationRay());
}

void LatLongEnvEmitter::calcDirectSamplePdfW(
	DirectEnergySamplePdfQuery& query,
	HitProbe& probe) const
{
	const math::Vector3R uvw(query.inputs.getXe().getDetail().getUVW());
	const math::Vector2R latLong01(uvw.x(), uvw.y());
	const auto latLong01Pdf = m_sampleDistribution.pdfContinuous({uvw.x(), uvw.y()});

	PrimitivePosSamplePdfQuery pdfQuery;
	pdfQuery.inputs.set(query.inputs);
	m_surface->calcPosSamplePdfWithObservationPos(latLong01, latLong01Pdf, pdfQuery, probe);
	if(!pdfQuery.outputs)
	{
		query.outputs.setPdfW(0);
		return;
	}

	query.outputs.setPdfW(lta::pdfA_to_pdfW(
		pdfQuery.outputs.getPdfA(),
		query.inputs.getTargetPos() - query.inputs.getEmitPos(),
		query.inputs.getXe().getShadingNormal()));
}

void LatLongEnvEmitter::emitRay(
	EnergyEmissionSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	real uvSamplePdf;
	const math::Vector2R uvSample = m_sampleDistribution.sampleContinuous(
		sampleFlow.flow2D(), &uvSamplePdf);

	const auto emittedEnergy = TSampler<math::Spectrum>{math::EColorUsage::EMR}.sample(
		*m_radiance, uvSample);

	PrimitivePosSampleQuery posSample;
	posSample.inputs.set(query.inputs.getTime());

	math::Vector3R unitObservationDir;
	real pdfW;
	m_surface->genPosSampleWithoutObservationPos(
		uvSample, uvSamplePdf, posSample, sampleFlow, probe, &unitObservationDir, &pdfW);
	if(!posSample.outputs)
	{
		query.outputs.invalidate();
		return;
	}

	auto emittedRay = posSample.outputs.getObservationRay();
	emittedRay.setDirection(-unitObservationDir);
	emittedRay.setRange(0, std::numeric_limits<real>::max());

	probe.replaceBaseHitRayTWith(emittedRay.getMinT());

	query.outputs.setEmittedEnergy(emittedEnergy);
	query.outputs.setEmittedRay(emittedRay);
	query.outputs.setPdf(posSample.outputs.getPdfA(), pdfW);
}

real LatLongEnvEmitter::calcRadiantFluxApprox() const
{
	PH_ASSERT(m_surface && m_radiance);

	return m_radiantFluxApprox;
}

}// end namespace ph
