#include "Core/Emitter/BackgroundEmitter.h"
#include "Common/assertion.h"
#include "Core/SurfaceHit.h"
#include "Core/Texture/TSampler.h"
#include "Common/Logger.h"

#include <vector>

namespace ph
{

namespace
{
	const Logger logger(LogSender("Background Emitter"));
}

BackgroundEmitter::BackgroundEmitter(
	const RadianceTexture&       radiance,
	const TVector2<std::size_t>& resolution) :

	m_radiance(radiance),
	m_sampleDistribution()
{
	PH_ASSERT(radiance && resolution.x * resolution.y > 0);

	logger.log(ELogLevel::NOTE_MED, 
		"constructing sample distribution with resolution " + resolution.toString());

	TSampler<SpectralStrength> sampler(EQuantity::EMR);
	std::vector<real>          sampleWeights(resolution.x * resolution.y);
	for(std::size_t y = 0; y < resolution.y; ++y)
	{
		const std::size_t baseIndex = y * resolution.x;
		const real v = (static_cast<real>(y) + 0.5_r) / static_cast<real>(resolution.y);
		for(std::size_t x = 0; x < resolution.x; ++x)
		{
			const real u = (static_cast<real>(x) + 0.5_r) / static_cast<real>(resolution.x);
			const SpectralStrength energy = sampler.sample(*radiance, {u, v});
			sampleWeights[baseIndex + x] = energy.calcLuminance();
		}
	}

	m_sampleDistribution = TPwcDistribution2D<real>(sampleWeights.data(), resolution);
}

void BackgroundEmitter::evalEmittedRadiance(
	const SurfaceHit&       X, 
	SpectralStrength* const out_radiance) const
{
	PH_ASSERT(out_radiance && m_radiance);

	const Vector3R emitDir = X.getIncidentRay().getDirection().mul(-1.0_r);
	if(!canEmit(emitDir, X.getShadingNormal()))
	{
		out_radiance->setValues(0.0_r);
		return;
	}

	TSampler<SpectralStrength> sampler(EQuantity::EMR);
	*out_radiance = sampler.sample(*m_radiance, X);
}

void BackgroundEmitter::genDirectSample(DirectLightSample& sample) const
{

}

// FIXME: ray time
void BackgroundEmitter::genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const
{
	PH_ASSERT_UNREACHABLE_SECTION();
}

real BackgroundEmitter::calcDirectSamplePdfW(
	const SurfaceHit& emitPos, 
	const Vector3R&   targetPos) const
{
	return 0.0_r;
}

}// end namespace ph