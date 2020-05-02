#include "Core/Emitter/OmniModulatedEmitter.h"
#include "Common/assertion.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceHit.h"
#include "Core/Texture/TSampler.h"
#include "Core/Sample/DirectLightSample.h"

namespace ph
{

OmniModulatedEmitter::OmniModulatedEmitter(std::unique_ptr<Emitter> source) :
	m_source(std::move(source)),
	m_filter(nullptr)
{
	PH_ASSERT(m_source);
}

void OmniModulatedEmitter::evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* out_radiance) const
{
	PH_ASSERT(m_filter);

	m_source->evalEmittedRadiance(X, out_radiance);

	// TODO: early out when radiance = 0

	const math::Vector3R emitDirection = X.getIncidentRay().getDirection().mul(-1);

	math::Vector3R uv;
	m_dirToUv.directionToUvw(emitDirection, &uv);

	// HACK
	uv.y = 1.0_r - uv.y;

	const auto& filterValue = TSampler<SpectralStrength>(EQuantity::RAW).sample(*m_filter, uv);
	out_radiance->mulLocal(filterValue);
}

void OmniModulatedEmitter::genDirectSample(SampleFlow& sampleFlow, DirectLightSample& sample) const
{
	m_source->genDirectSample(sampleFlow, sample);
	if(sample.pdfW == 0.0_r)
	{
		return;
	}

	const math::Vector3R emitDirection = sample.targetPos.sub(sample.emitPos);

	math::Vector3R uv;
	m_dirToUv.directionToUvw(emitDirection, &uv);

	// HACK
	uv.y = 1.0_r - uv.y;

	const auto& filterValue = TSampler<SpectralStrength>(EQuantity::RAW).sample(*m_filter, uv);
	sample.radianceLe.mulLocal(filterValue);
}

void OmniModulatedEmitter::emitRay(SampleFlow& sampleFlow, Ray* out_ray, SpectralStrength* out_Le, math::Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const
{
	m_source->emitRay(sampleFlow, out_ray, out_Le, out_eN, out_pdfA, out_pdfW);

	math::Vector3R uv;
	m_dirToUv.directionToUvw(out_ray->getDirection(), &uv);

	// HACK
	uv.y = 1.0_r - uv.y;

	const auto& filterValue = TSampler<SpectralStrength>(EQuantity::RAW).sample(*m_filter, uv);
	out_Le->mulLocal(filterValue);
}

real OmniModulatedEmitter::calcDirectSamplePdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const
{
	const real pdfW = m_source->calcDirectSamplePdfW(emitPos, targetPos);

	// TODO: if importance sampling is used, pdfW should be changed here

	return pdfW;
}

void OmniModulatedEmitter::setFilter(const std::shared_ptr<TTexture<SpectralStrength>>& filter)
{
	PH_ASSERT(filter);

	m_filter = filter;
}

real OmniModulatedEmitter::calcRadiantFluxApprox() const
{
	PH_ASSERT(m_source);

	return m_source->calcRadiantFluxApprox();
}

}// end namespace ph
