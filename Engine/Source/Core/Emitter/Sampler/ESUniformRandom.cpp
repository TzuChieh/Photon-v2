#include "Core/Emitter/Sampler/ESUniformRandom.h"
#include "Actor/AModel.h"
#include "Actor/ALight.h"
#include "Math/Random.h"
#include "Actor/CookedDataStorage.h"
#include "Core/Sample/DirectLightSample.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Emitter/Emitter.h"
#include "Common/assertion.h"

#include <iostream>

namespace ph
{

void ESUniformRandom::update(const CookedDataStorage& cookedActors)
{
	m_emitters.clear();
	m_emitters.shrink_to_fit();

	for(const auto& emitter : cookedActors.emitters())
	{
		m_emitters.push_back(emitter.get());
	}

	if(m_emitters.empty())
	{
		std::cerr << "warning: at ESUniformRandom::update(), no Emitter detected" << std::endl;
	}
}

const Emitter* ESUniformRandom::pickEmitter(SampleFlow& sampleFlow, real* const out_PDF) const
{
	const std::size_t picker = static_cast<std::size_t>(math::Random::genUniformReal_i0_e1() * static_cast<real>(m_emitters.size()));
	const std::size_t pickedIndex = picker == m_emitters.size() ? picker - 1 : picker;

	*out_PDF = 1.0_r / static_cast<real>(m_emitters.size());
	return m_emitters[pickedIndex];
}

void ESUniformRandom::genDirectSample(SampleFlow& sampleFlow, DirectLightSample& sample) const
{
	// randomly and uniformly select an emitter
	// FIXME: use sampleFlow
	const std::size_t picker = static_cast<std::size_t>(sampleFlow.flow1D() * static_cast<real>(m_emitters.size()));
	const std::size_t pickedIndex = picker == m_emitters.size() ? picker - 1 : picker;
	const real pickPdfW = 1.0_r / static_cast<real>(m_emitters.size());

	m_emitters[pickedIndex]->genDirectSample(sampleFlow, sample);
	sample.pdfW *= pickPdfW;
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
