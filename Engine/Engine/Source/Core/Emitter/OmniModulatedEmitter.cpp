#include "Core/Emitter/OmniModulatedEmitter.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/SurfaceHit.h"
#include "Core/Texture/TSampler.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Emitter/Query/EnergyEmissionSampleQuery.h"

#include <Common/assertion.h>

namespace ph
{

OmniModulatedEmitter::OmniModulatedEmitter(
	const Emitter* const    source,
	const EmitterFeatureSet featureSet)

	: Emitter(featureSet)

	, m_source(source)
	, m_filter(nullptr)
{
	PH_ASSERT(m_source);
}

void OmniModulatedEmitter::evalEmittedEnergy(const SurfaceHit& X, math::Spectrum* const out_energy) const
{
	m_source->evalEmittedEnergy(X, out_energy);
	if(out_energy->isZero())
	{
		return;
	}

	const math::Vector3R emitDir = X.getIncidentRay().getDir().mul(-1);

	math::Vector3R uv;
	m_dirToUv.dirToUvw(emitDir, &uv);

	// HACK: should impose some standard uv on input
	uv.y() = 1.0_r - uv.y();

	PH_ASSERT(m_filter);
	const auto& filterValue = TSampler<math::Spectrum>(math::EColorUsage::RAW).sample(*m_filter, uv);
	out_energy->mulLocal(filterValue);
}

void OmniModulatedEmitter::genDirectSample(
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	if(getFeatureSet().hasNo(EEmitterFeatureSet::DirectSample))
	{
		return;
	}

	m_source->genDirectSample(query, sampleFlow, probe);
	if(!query.outputs)
	{
		return;
	}

	const auto emitDir = query.inputs.getTargetPos() - query.outputs.getEmitPos();

	math::Vector3R uv;
	m_dirToUv.dirToUvw(emitDir, &uv);

	// HACK: should impose some standard uv on input
	uv.y() = 1.0_r - uv.y();

	const auto& filterValue = TSampler<math::Spectrum>(math::EColorUsage::RAW).sample(*m_filter, uv);
	query.outputs.setEmittedEnergy(query.outputs.getEmittedEnergy() * filterValue);
}

void OmniModulatedEmitter::calcDirectPdf(DirectEnergyPdfQuery& query) const
{
	if(getFeatureSet().hasNo(EEmitterFeatureSet::DirectSample))
	{
		return;
	}

	m_source->calcDirectPdf(query);

	// TODO: if importance sampling is used, pdfW should be changed here
}

void OmniModulatedEmitter::emitRay(
	EnergyEmissionSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	if(getFeatureSet().hasNo(EEmitterFeatureSet::EmissionSample))
	{
		return;
	}

	m_source->emitRay(query, sampleFlow, probe);
	if(!query.outputs)
	{
		return;
	}

	math::Vector3R uv;
	m_dirToUv.dirToUvw(query.outputs.getEmittedRay().getDir(), &uv);

	// HACK: should impose some standard uv on input
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
