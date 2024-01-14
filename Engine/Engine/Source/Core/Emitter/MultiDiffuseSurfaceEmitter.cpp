#include "Core/Emitter/MultiDiffuseSurfaceEmitter.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Texture/TSampler.h"
#include "Math/Color/Spectrum.h"
#include "Core/Texture/TTexture.h"
#include "Math/Random/Random.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"

#include <Common/assertion.h>
#include <Common/utility.h>

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

void MultiDiffuseSurfaceEmitter::evalEmittedRadiance(const SurfaceHit& X, math::Spectrum* const out_radiance) const
{
	PH_ASSERT(!m_emitters.empty());

	m_emitters.front().evalEmittedRadiance(X, out_radiance);
}

void MultiDiffuseSurfaceEmitter::genDirectSample(DirectEnergySampleQuery& query, SampleFlow& sampleFlow) const
{
	PH_ASSERT(!m_emitters.empty());

	// FIXME: use sampleFlow
	const DiffuseSurfaceEmitter& emitter = m_emitters[math::Random::index(0, m_emitters.size())];

	emitter.genDirectSample(query, sampleFlow);
	const real pickPdf = (1.0_r / static_cast<real>(m_emitters.size()));
	query.out.pdfW *= pickPdf;
}

void MultiDiffuseSurfaceEmitter::emitRay(SampleFlow& sampleFlow, Ray* out_ray, math::Spectrum* out_Le, math::Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const
{
	// randomly and uniformly pick a primitive

	// FIXME: use sampleFlow
	const auto& emitter = m_emitters[math::Random::index(0, m_emitters.size())];
	const real pickPdf = 1.0_r / static_cast<real>(m_emitters.size());

	emitter.emitRay(sampleFlow, out_ray, out_Le, out_eN, out_pdfA, out_pdfW);
	*out_pdfA *= pickPdf;



	/*PositionSample tPositionSample;
	m_localToWorld->transformP(positionSample.position, &tPositionSample.position);
	m_localToWorld->transformO(positionSample.normal, &tPositionSample.normal);
	tPositionSample.normal.normalizeLocal();
	tPositionSample.uvw = positionSample.uvw;
	tPositionSample.pdf = positionSample.pdf;*/

	// DEBUG
	//tPositionSample = positionSample;

	// random & uniform direction on a unit sphere
	/*Vector3R rayDir;
	const real r1 = Random::genUniformReal_i0_e1();
	const real r2 = Random::genUniformReal_i0_e1();
	const real sqrtTerm = std::sqrt(r2 * (1.0_r - r2));
	const real anglTerm = 2.0_r * PH_PI_REAL * r1;
	rayDir.x = 2.0_r * std::cos(anglTerm) * sqrtTerm;
	rayDir.y = 2.0_r * std::sin(anglTerm) * sqrtTerm;
	rayDir.z = 1.0_r - 2.0_r * r2;
	rayDir.normalizeLocal();*/

	// TODO: time

	//out_ray->setDirection(rayDir);
	//out_ray->setOrigin(tPositionSample.position);
	//out_ray->setMinT(0.0001_r);// HACK: hard-code number
	//out_ray->setMaxT(Ray::MAX_T);
	//out_eN->set(tPositionSample.normal);
	//*out_pdfA = pickPdfW * tPositionSample.pdf;
	//*out_pdfW = 1.0_r / (4.0_r * PH_PI_REAL) / out_ray->getDirection().absDot(tPositionSample.normal);
	//m_emittedRadiance->sample(tPositionSample.uvw, out_Le);

	//std::cerr << "PrimitiveAreaEmitter::emitRay() not implemented" << std::endl;
}

real MultiDiffuseSurfaceEmitter::calcDirectSamplePdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const
{
	PH_ASSERT(!m_emitters.empty());

	const real singlePdfW = calcPdfW(emitPos, targetPos);
	const real pickPdf    = (1.0_r / static_cast<real>(m_emitters.size()));
	return singlePdfW * pickPdf;
}

void MultiDiffuseSurfaceEmitter::setEmittedRadiance(
	const std::shared_ptr<TTexture<math::Spectrum>>& emittedRadiance)
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

const TTexture<math::Spectrum>& MultiDiffuseSurfaceEmitter::getEmittedRadiance() const
{
	PH_ASSERT(!m_emitters.empty());

	return m_emitters.front().getEmittedRadiance();
}

real MultiDiffuseSurfaceEmitter::calcRadiantFluxApprox() const
{
	real totalRadiantFlux = 0.0_r;
	for(const auto& emitter : m_emitters)
	{
		totalRadiantFlux += emitter.calcRadiantFluxApprox();
	}

	return totalRadiantFlux > 0.0_r ? totalRadiantFlux : SurfaceEmitter::calcRadiantFluxApprox();
}

}// end namespace ph
