#include "Engine/Core/Emitter/TOmniModulatedEmitter.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Texture/TSampler.h"
#include "Engine/Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Engine/Core/Emitter/Query/EnergyEmissionSampleQuery.h"

#include <Common/assertion.h>

// TODO: importance sampling based on filter values

namespace ph
{

template<typename SourceEmitter>
inline TOmniModulatedEmitter<SourceEmitter>::TOmniModulatedEmitter(
	const SourceEmitter* const source)

	: TOmniModulatedEmitter(
		source,
		source ? source->getFeatureSet() : Emitter::defaultFeatureSet)
{}

template<typename SourceEmitter>
inline TOmniModulatedEmitter<SourceEmitter>::TOmniModulatedEmitter(
	const SourceEmitter* const source,
	const EmitterFeatureSet featureSet)

	: SourceEmitter(featureSet)

	, m_source(source)
	, m_filter(nullptr)
{
	PH_ASSERT(source);
}

template<typename SourceEmitter>
inline const SourceEmitter& TOmniModulatedEmitter<SourceEmitter>::getSource() const
{
	PH_ASSERT(m_source);
	return *m_source;
}

template<typename SourceEmitter>
inline void TOmniModulatedEmitter<SourceEmitter>::evalEmittedEnergy(const SurfaceHit& Xe, math::Spectrum* const out_energy) const
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
	const auto filterValue = TSampler<math::Spectrum>().sample(*m_filter, uv);
	out_energy->mulLocal(filterValue);
}

template<typename SourceEmitter>
inline void TOmniModulatedEmitter<SourceEmitter>::genDirectSample(
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	if(this->getFeatureSet().hasNo(EEmitterFeatureSet::DirectSample))
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

	const auto filterValue = TSampler<math::Spectrum>().sample(*m_filter, uv);
	query.outputs.setEmittedEnergy(query.outputs.getEmittedEnergy() * filterValue);
}

template<typename SourceEmitter>
inline void TOmniModulatedEmitter<SourceEmitter>::calcDirectPdf(DirectEnergyPdfQuery& query) const
{
	if(this->getFeatureSet().hasNo(EEmitterFeatureSet::DirectSample))
	{
		return;
	}

	getSource().calcDirectPdf(query);

	// TODO: if importance sampling is used, pdfW should be changed here
}

template<typename SourceEmitter>
inline void TOmniModulatedEmitter<SourceEmitter>::emitRay(
	EnergyEmissionSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	if(this->getFeatureSet().hasNo(EEmitterFeatureSet::EmissionSample))
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

	const auto filterValue = TSampler<math::Spectrum>().sample(*m_filter, uv);
	query.outputs.setEmittedEnergy(query.outputs.getEmittedEnergy() * filterValue);
}

template<typename SourceEmitter>
inline void TOmniModulatedEmitter<SourceEmitter>::setFilter(const std::shared_ptr<TTexture<math::Spectrum>>& filter)
{
	PH_ASSERT(filter);
	m_filter = filter;
}

template<typename SourceEmitter>
inline real TOmniModulatedEmitter<SourceEmitter>::calcRadiantFluxApprox() const
{
	return getSource().calcRadiantFluxApprox();
}

}// end namespace ph
