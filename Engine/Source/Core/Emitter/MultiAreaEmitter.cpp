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
	// TODO: able to specify channel or restrict it
	TSampler<SpectralStrength> sampler(EQuantity::EMR);
	*out_radiance = sampler.sample(*m_emittedRadiance, X);
}

void MultiAreaEmitter::genDirectSample(
	const Vector3R& targetPos, 
	Vector3R* out_emitPos, 
	SpectralStrength* out_emittedRadiance, 
	real* out_PDF) const
{
	const PrimitiveAreaEmitter& emitter = m_areaEmitters[Random::genUniformIndex_iL_eU(0, m_areaEmitters.size())];

	emitter.genDirectSample(targetPos, out_emitPos, out_emittedRadiance, out_PDF);
	const real pickPdf = (1.0_r / static_cast<real>(m_areaEmitters.size()));
	(*out_PDF) *= pickPdf;
}

void MultiAreaEmitter::genDirectSample(DirectLightSample& sample) const
{
	const PrimitiveAreaEmitter& emitter = m_areaEmitters[Random::genUniformIndex_iL_eU(0, m_areaEmitters.size())];

	emitter.genDirectSample(sample);
	const real pickPdf = (1.0_r / static_cast<real>(m_areaEmitters.size()));
	sample.pdfW *= pickPdf;
}

void MultiAreaEmitter::genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

real MultiAreaEmitter::calcDirectSamplePdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Primitive* hitPrim) const
{
	const PrimitiveAreaEmitter& emitter = m_areaEmitters[Random::genUniformIndex_iL_eU(0, m_areaEmitters.size())];

	const real pdfW    = emitter.calcDirectSamplePdfW(targetPos, emitPos, emitN, hitPrim);
	const real pickPdf = (1.0_r / static_cast<real>(m_areaEmitters.size()));
	return pdfW * pickPdf;
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