#include "Core/Emitter/Sampler/ESUniformRandom.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Emitter/Query/DirectEnergySamplePdfQuery.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Emitter/Emitter.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>

#include <iostream>
#include <cmath>

namespace ph
{

void ESUniformRandom::update(TSpanView<const Emitter*> emitters)
{
	m_emitters.clear();
	m_emitters.shrink_to_fit();

	for(const auto& emitter : emitters)
	{
		m_emitters.push_back(emitter);
	}

	if(m_emitters.empty())
	{
		std::cerr << "warning: at ESUniformRandom::update(), no Emitter detected" << std::endl;
	}
}

const Emitter* ESUniformRandom::pickEmitter(SampleFlow& sampleFlow, real* const out_PDF) const
{
	// FIXME: use sampleFlow for index
	const std::size_t picker = static_cast<std::size_t>(sampleFlow.flow1D() * static_cast<real>(m_emitters.size()));
	const std::size_t pickedIndex = picker == m_emitters.size() ? picker - 1 : picker;

	*out_PDF = 1.0_r / static_cast<real>(m_emitters.size());
	return m_emitters[pickedIndex];
}

void ESUniformRandom::genDirectSample(
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	// FIXME: use sampleFlow for index
	// Randomly and uniformly select an emitter
	const std::size_t picker = static_cast<std::size_t>(sampleFlow.flow1D() * static_cast<real>(m_emitters.size()));
	const std::size_t pickedIndex = picker == m_emitters.size() ? picker - 1 : picker;
	const real pickPdfW = 1.0_r / static_cast<real>(m_emitters.size());

	m_emitters[pickedIndex]->genDirectSample(query, sampleFlow, probe);
	if(!query.outputs)
	{
		return;
	}

	query.outputs.setPdfW(query.outputs.getPdfW() * pickPdfW);
}

void ESUniformRandom::calcDirectSamplePdfW(
	DirectEnergySamplePdfQuery& query,
	HitProbe& probe) const
{
	const Primitive* const hitPrim = query.inputs.getSrcPrimitive();
	PH_ASSERT(hitPrim);
	const Emitter* const hitEmitter = hitPrim->getMetadata()->getSurface().getEmitter();
	PH_ASSERT(hitEmitter);

	hitEmitter->calcDirectSamplePdfW(query, probe);
	if(!query.outputs)
	{
		return;
	}

	const auto pickPdf = 1.0_r / static_cast<real>(m_emitters.size());
	const auto directPdfW = query.outputs.getPdfW() * pickPdf;
	query.outputs.setPdfW(std::isfinite(directPdfW) ? directPdfW : 0);
}

}// end namespace ph
