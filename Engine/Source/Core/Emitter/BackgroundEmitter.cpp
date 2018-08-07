#include "Core/Emitter/BackgroundEmitter.h"
#include "Common/assertion.h"
#include "Core/SurfaceHit.h"
#include "Core/Texture/TSampler.h"
#include "Common/Logger.h"
#include "Core/Intersectable/UvwMapper/SphericalMapper.h"
#include "Math/Random.h"
#include "Core/Sample/DirectLightSample.h"
#include "Core/Intersectable/Primitive.h"
#include "Math/constant.h"

#include <vector>

namespace ph
{

namespace
{
	const Logger logger(LogSender("Background Emitter"));
}

BackgroundEmitter::BackgroundEmitter(
	const Primitive* const       surface,
	const RadianceTexture&       radiance,
	const TVector2<std::size_t>& resolution,
	const AABB3D&                worldBound) :

	m_surface(surface),
	m_radiance(radiance),
	m_sampleDistribution(),
	m_worldBound(worldBound)
{
	PH_ASSERT(surface && radiance && resolution.x * resolution.y > 0);

	logger.log(ELogLevel::NOTE_MED, 
		"constructing sample distribution with resolution " + resolution.toString());

	const EQuantity            quantity = EQuantity::EMR;
	TSampler<SpectralStrength> sampler(quantity);
	std::vector<real>          sampleWeights(resolution.x * resolution.y);
	for(std::size_t y = 0; y < resolution.y; ++y)
	{
		const std::size_t baseIndex = y * resolution.x;
		const real v        = (static_cast<real>(y) + 0.5_r) / static_cast<real>(resolution.y);
		const real sinTheta = std::sin(v * PH_PI_REAL);
		for(std::size_t x = 0; x < resolution.x; ++x)
		{
			const real u = (static_cast<real>(x) + 0.5_r) / static_cast<real>(resolution.x);
			const SpectralStrength energy = sampler.sample(*radiance, {u, v});
			sampleWeights[baseIndex + x] = energy.calcLuminance(quantity) * sinTheta;
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
	sample.pdfW = 0;
	sample.sourcePrim = m_surface;

	real uvSamplePdf;
	const Vector2R uvSample = m_sampleDistribution.sampleContinuous(
		Random::genUniformReal_i0_e1(),
		Random::genUniformReal_i0_e1(),
		&uvSamplePdf);

	Vector3R position;
	if(!m_surface->uvwToPosition(Vector3R(uvSample.x, uvSample.y, 0), &position))
	{
		return;
	}
	sample.emitPos = sample.targetPos + position;

	TSampler<SpectralStrength> sampler(EQuantity::EMR);
	sample.radianceLe = sampler.sample(*m_radiance, uvSample);
	
	const real sinTheta = std::sin(uvSample.y * PH_PI_REAL);
	if(sinTheta == 0.0_r)
	{
		return;
	}
	sample.pdfW = uvSamplePdf / (2.0_r * PH_PI_REAL * PH_PI_REAL * sinTheta);
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
	/*const Vector3R L = emitPos.getPosition().sub(targetPos).normalize();
	
	Vector3R uvw;
	SphericalMapper mapper;
	mapper.map(L, &uvw);*/

	const Vector3R uvw = emitPos.getDetail().getUvw();
	const real sinTheta = std::sin(uvw.y * PH_PI_REAL);
	if(sinTheta == 0.0_r)
	{
		return 0.0_r;
	}
	return m_sampleDistribution.pdf({uvw.x, uvw.y}) / (2.0_r * PH_PI_REAL * PH_PI_REAL * sinTheta);
}

}// end namespace ph