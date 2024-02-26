#include "Core/Emitter/OmniModulatedEmitter.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/SurfaceHit.h"
#include "Core/Texture/TSampler.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Emitter/Query/EnergyEmissionSampleQuery.h"

#include <Common/assertion.h>

namespace ph
{

OmniModulatedEmitter::OmniModulatedEmitter(const Emitter* source)
	: m_source(source)
	, m_filter(nullptr)
{
	PH_ASSERT(m_source);
}

void OmniModulatedEmitter::evalEmittedRadiance(const SurfaceHit& X, math::Spectrum* out_radiance) const
{
	PH_ASSERT(m_filter);

	m_source->evalEmittedRadiance(X, out_radiance);

	// TODO: early out when radiance = 0

	const math::Vector3R emitDirection = X.getIncidentRay().getDirection().mul(-1);

	math::Vector3R uv;
	m_dirToUv.directionToUvw(emitDirection, &uv);

	// HACK
	uv.y() = 1.0_r - uv.y();

	const auto& filterValue = TSampler<math::Spectrum>(math::EColorUsage::RAW).sample(*m_filter, uv);
	out_radiance->mulLocal(filterValue);
}

void OmniModulatedEmitter::genDirectSample(
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	m_source->genDirectSample(query, sampleFlow, probe);
	if(!query.outputs)
	{
		return;
	}

	const auto emitDirection = query.inputs.getTargetPos() - query.outputs.getEmitPos();

	math::Vector3R uv;
	m_dirToUv.directionToUvw(emitDirection, &uv);

	// HACK
	uv.y() = 1.0_r - uv.y();

	const auto& filterValue = TSampler<math::Spectrum>(math::EColorUsage::RAW).sample(*m_filter, uv);
	query.outputs.setEmittedEnergy(query.outputs.getEmittedEnergy() * filterValue);
}

void OmniModulatedEmitter::calcDirectSamplePdfW(
	DirectEnergySamplePdfQuery& query,
	HitProbe& probe) const
{
	m_source->calcDirectSamplePdfW(query, probe);

	// TODO: if importance sampling is used, pdfW should be changed here
}

void OmniModulatedEmitter::emitRay(
	EnergyEmissionSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	m_source->emitRay(query, sampleFlow, probe);
	if(!query.outputs)
	{
		return;
	}

	math::Vector3R uv;
	m_dirToUv.directionToUvw(query.outputs.getEmittedRay().getDirection(), &uv);

	// HACK
	uv.y() = 1.0_r - uv.y();

	const auto filterValue = TSampler<math::Spectrum>(math::EColorUsage::RAW).sample(*m_filter, uv);
	query.outputs.setEmittedEnergy(query.outputs.getEmittedEnergy() * filterValue);
}

void OmniModulatedEmitter::setFilter(const std::shared_ptr<TTexture<math::Spectrum>>& filter)
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
