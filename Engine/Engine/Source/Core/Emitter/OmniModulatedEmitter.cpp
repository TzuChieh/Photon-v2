#include "Core/Emitter/OmniModulatedEmitter.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/SurfaceHit.h"
#include "Core/Texture/TSampler.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Emitter/Query/EnergyEmissionSampleQuery.h"

// TODO: importance sampling based on filter values

namespace ph
{

OmniModulatedEmitter::OmniModulatedEmitter(
	const Emitter* const source)

	: OmniModulatedEmitter(
		source,
		source ? source->getFeatureSet() : defaultFeatureSet)
{}

OmniModulatedEmitter::OmniModulatedEmitter(
	const Emitter* const    source,
	const EmitterFeatureSet featureSet)

	: Emitter(featureSet)

	, m_source(source)
	, m_filter(nullptr)
{
	PH_ASSERT(source);
}

void OmniModulatedEmitter::evalEmittedEnergy(const SurfaceHit& Xe, math::Spectrum* const out_energy) const
{
	getSource().evalEmittedEnergy(Xe, out_energy);
	if(out_energy->isZero())
	{
		return;
	}

	const math::Vector3R emitDir = Xe.getIncidentRay().getDir().mul(-1);

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

	getSource().genDirectSample(query, sampleFlow, probe);
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

	getSource().calcDirectPdf(query);

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

	getSource().emitRay(query, sampleFlow, probe);
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
	return getSource().calcRadiantFluxApprox();
}

}// end namespace ph
