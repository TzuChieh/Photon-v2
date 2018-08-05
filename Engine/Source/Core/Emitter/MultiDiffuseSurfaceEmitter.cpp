#include "Core/Emitter/MultiDiffuseSurfaceEmitter.h"
#include "Common/assertion.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Texture/TSampler.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Texture/TTexture.h"
#include "Common/utility.h"
#include "Math/Random.h"
#include "Core/Sample/PositionSample.h"
#include "Core/Sample/DirectLightSample.h"

#include <iostream>

namespace ph
{

MultiDiffuseSurfaceEmitter::MultiDiffuseSurfaceEmitter(const std::vector<DiffuseSurfaceEmitter>& emitters) :
	SurfaceEmitter(),
	m_emitters()
{
	PH_ASSERT(!emitters.empty());

	m_extendedArea = 0.0_r;
	for(const auto& emitter : emitters)
	{
		const Primitive* primitive = emitter.getSurface();

		PH_ASSERT(primitive != nullptr);
		m_extendedArea += primitive->calcExtendedArea();

		addEmitter(emitter);
	}
	m_reciExtendedArea = 1.0_r / m_extendedArea;
}

void MultiDiffuseSurfaceEmitter::evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* const out_radiance) const
{
	PH_ASSERT(!m_emitters.empty());

	m_emitters.front().evalEmittedRadiance(X, out_radiance);
}

void MultiDiffuseSurfaceEmitter::genDirectSample(DirectLightSample& sample) const
{
	PH_ASSERT(!m_emitters.empty());

	const DiffuseSurfaceEmitter& emitter = m_emitters[Random::genUniformIndex_iL_eU(0, m_emitters.size())];

	emitter.genDirectSample(sample);
	const real pickPdf = (1.0_r / static_cast<real>(m_emitters.size()));
	sample.pdfW *= pickPdf;
}

void MultiDiffuseSurfaceEmitter::genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

real MultiDiffuseSurfaceEmitter::calcDirectSamplePdfW(const SurfaceHit& emitPos, const Vector3R& targetPos) const
{
	PH_ASSERT(!m_emitters.empty());

	const real singlePdfW = calcPdfW(emitPos, targetPos);
	const real pickPdf    = (1.0_r / static_cast<real>(m_emitters.size()));
	return singlePdfW * pickPdf;
}

void MultiDiffuseSurfaceEmitter::setEmittedRadiance(
	const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance)
{
	for(auto& emitter : m_emitters)
	{
		emitter.setEmittedRadiance(emittedRadiance);
	}
}

void MultiDiffuseSurfaceEmitter::addEmitter(const DiffuseSurfaceEmitter& emitter)
{
	m_emitters.push_back(emitter);

	if(m_isBackFaceEmission)
	{
		m_emitters.back().setBackFaceEmit();
	}
	else
	{
		m_emitters.back().setFrontFaceEmit();
	}
}

void MultiDiffuseSurfaceEmitter::setFrontFaceEmit()
{
	SurfaceEmitter::setFrontFaceEmit();

	for(auto& emitter : m_emitters)
	{
		emitter.setFrontFaceEmit();
	}
}

void MultiDiffuseSurfaceEmitter::setBackFaceEmit()
{
	SurfaceEmitter::setBackFaceEmit();

	for(auto& emitter : m_emitters)
	{
		emitter.setBackFaceEmit();
	}
}

const TTexture<SpectralStrength>& MultiDiffuseSurfaceEmitter::getEmittedRadiance() const
{
	PH_ASSERT(!m_emitters.empty());

	return m_emitters.front().getEmittedRadiance();
}

}// end namespace ph