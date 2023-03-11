#include "Core/Emitter/Sampler/ESUniformRandom.h"
#include "Math/Random.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Emitter/Emitter.h"
#include "Common/assertion.h"
#include "Core/SampleGenerator/SampleFlow.h"
#include "Core/Intersectable/PrimitiveMetadata.h"

#include <iostream>

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

void ESUniformRandom::genDirectSample(DirectEnergySampleQuery& query, SampleFlow& sampleFlow) const
{
	// randomly and uniformly select an emitter
	// FIXME: use sampleFlow for index
	const std::size_t picker = static_cast<std::size_t>(sampleFlow.flow1D() * static_cast<real>(m_emitters.size()));
	const std::size_t pickedIndex = picker == m_emitters.size() ? picker - 1 : picker;
	const real pickPdfW = 1.0_r / static_cast<real>(m_emitters.size());

	m_emitters[pickedIndex]->genDirectSample(query, sampleFlow);
	query.out.pdfW *= pickPdfW;
}

real ESUniformRandom::calcDirectPdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const
{
	const real pickPdfW = 1.0_r / static_cast<real>(m_emitters.size());

	const Primitive* const hitPrim = emitPos.getDetail().getPrimitive();
	PH_ASSERT(hitPrim);
	const Emitter* const hitEmitter = hitPrim->getMetadata()->getSurface().getEmitter();
	PH_ASSERT(hitEmitter);
	const real samplePdfW = hitEmitter->calcDirectSamplePdfW(emitPos, targetPos);

	return pickPdfW * samplePdfW;
}

}// end namespace ph
