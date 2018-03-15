#include "Core/Emitter/MultiAreaEmitter.h"
#include "Common/assertion.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Texture/TSampler.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Texture/TTexture.h"
#include "Common/utility.h"

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
	TSampler<SpectralStrength> sampler(EQuantity::EMR);
	*out_radiance = sampler.sample(*m_emittedRadiance, X);
}

void MultiAreaEmitter::genDirectSample(const Vector3R& targetPos, Vector3R* out_emitPos, SpectralStrength* out_emittedRadiance, real* out_PDF) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

void MultiAreaEmitter::genDirectSample(DirectLightSample& sample) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

void MultiAreaEmitter::genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

real MultiAreaEmitter::calcDirectSamplePdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Primitive* hitPrim) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
	return 0.0_r;
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