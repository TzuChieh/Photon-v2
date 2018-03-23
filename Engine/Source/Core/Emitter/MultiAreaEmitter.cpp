#include "Core/Emitter/MultiAreaEmitter.h"
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

MultiAreaEmitter::MultiAreaEmitter(const std::vector<PrimitiveAreaEmitter>& areaEmitters) :
	Emitter(),
	m_areaEmitters(areaEmitters),
	m_emittedRadiance(nullptr)
{
	PH_ASSERT(!areaEmitters.empty());

	m_extendedArea = 0.0_r;
	for(const auto& areaEmitter : areaEmitters)
	{
		const Primitive* primitive = areaEmitter.getPrimitive();
		PH_ASSERT(primitive != nullptr);

		m_extendedArea += primitive->calcExtendedArea();
	}
	m_reciExtendedArea = 1.0_r / m_extendedArea;
}

MultiAreaEmitter::~MultiAreaEmitter() = default;

void MultiAreaEmitter::evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* out_radiance) const
{
	// FIXME: sort of hacked... (the direction of ray is reversed)
	// only front side of the emitter is emissive
	if(X.getIncidentRay().getDirection().mul(-1.0_r).dot(X.getShadingNormal()) <= 0.0_r)
	{
		out_radiance->setValues(0.0_r);
		return;
	}

	// TODO: able to specify channel or restrict it
	TSampler<SpectralStrength> sampler(EQuantity::EMR);
	*out_radiance = sampler.sample(*m_emittedRadiance, X);
}

void MultiAreaEmitter::genDirectSample(DirectLightSample& sample) const
{
	const PrimitiveAreaEmitter& emitter = m_areaEmitters[Random::genUniformIndex_iL_eU(0, m_areaEmitters.size())];

	emitter.genDirectSample(sample);
	const real pickPdf = (1.0_r / static_cast<real>(m_areaEmitters.size()));
	sample.pdfW *= pickPdf;



	std::cerr << sample.emitPos.toString() << std::endl;
}

void MultiAreaEmitter::genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

real MultiAreaEmitter::calcDirectSamplePdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Primitive* hitPrim) const
{
	// HACK

	const Vector3R emitDir(targetPos.sub(emitPos).normalizeLocal());
	const real emitDirDotNormal = emitDir.dot(emitN);
	if(emitDirDotNormal <= 0.0_r)
	{
		return 0.0_r;
	}
	
	const real pickPdf = (1.0_r / static_cast<real>(m_areaEmitters.size()));
	const real samplePdfA  = hitPrim->calcPositionSamplePdfA(emitPos);
	const real distSquared = targetPos.sub(emitPos).lengthSquared();
	return samplePdfA / std::abs(emitDirDotNormal) * distSquared * pickPdf;

	/*const PrimitiveAreaEmitter& emitter = m_areaEmitters[Random::genUniformIndex_iL_eU(0, m_areaEmitters.size())];
	const real pdfW    = emitter.calcDirectSamplePdfW(targetPos, emitPos, emitN, hitPrim);
	const real pickPdf = (1.0_r / static_cast<real>(m_areaEmitters.size()));
	return pdfW * pickPdf;*/
}

void MultiAreaEmitter::setEmittedRadiance(const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance)
{
	PH_ASSERT(emittedRadiance != nullptr);

	for(auto& areaEmitter : m_areaEmitters)
	{
		areaEmitter.setEmittedRadiance(emittedRadiance);
	}
	m_emittedRadiance = emittedRadiance;
}

}// end namespace ph