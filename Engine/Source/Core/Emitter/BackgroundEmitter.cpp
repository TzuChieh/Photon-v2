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
#include "Math/math.h"
#include "Math/Mapping/UniformUnitDisk.h"
#include "Math/TOrthonormalBasis3.h"

#include <vector>

namespace ph
{

namespace
{
	const Logger logger(LogSender("Background Emitter"));
}

BackgroundEmitter::BackgroundEmitter(
	const Primitive* const             surface,
	const RadianceTexture&             radiance,
	const math::TVector2<std::size_t>& resolution,
	real sceneBoundRadius) :

	m_surface(surface),
	m_radiance(radiance),
	m_sampleDistribution(),
	m_radiantFluxApprox(0),

	m_sceneBoundRadius(sceneBoundRadius)
{
	PH_ASSERT(surface && radiance && resolution.x * resolution.y > 0);

	logger.log(ELogLevel::NOTE_MED, 
		"constructing sample distribution with resolution " + resolution.toString());

	// FIXME: assuming spherical uv mapping us used

	constexpr EQuantity QUANTITY = EQuantity::EMR;
	const TSampler<SpectralStrength> sampler(QUANTITY);

	std::vector<real> sampleWeights(resolution.x * resolution.y);
	for(std::size_t y = 0; y < resolution.y; ++y)
	{
		const std::size_t baseIndex = y * resolution.x;
		const real v        = (static_cast<real>(y) + 0.5_r) / static_cast<real>(resolution.y);
		const real sinTheta = std::sin((1.0_r - v) * math::constant::pi<real>);
		for(std::size_t x = 0; x < resolution.x; ++x)
		{
			const real u = (static_cast<real>(x) + 0.5_r) / static_cast<real>(resolution.x);
			const SpectralStrength sampledL = sampler.sample(*radiance, {u, v});

			// FIXME: for non-nearest filtered textures, sample weights can be 0 while
			// there is still energay around that point (because its neighbor may have
			// non-zero energy), this can cause rendering artifacts
			sampleWeights[baseIndex + x] = sampledL.calcLuminance(QUANTITY) * sinTheta;

			m_radiantFluxApprox += sampleWeights[baseIndex + x];
		}
	}

	m_sampleDistribution = math::TPwcDistribution2D<real>(sampleWeights.data(), resolution);

	//m_radiantFluxApprox = m_radiantFluxApprox * m_surface->calcExtendedArea() * PH_PI_REAL;
	m_radiantFluxApprox  = m_radiantFluxApprox * 4 * m_sceneBoundRadius * m_sceneBoundRadius * math::constant::pi<real>;
}

void BackgroundEmitter::evalEmittedRadiance(
	const SurfaceHit&       X, 
	SpectralStrength* const out_radiance) const
{
	PH_ASSERT(out_radiance && m_radiance);

	TSampler<SpectralStrength> sampler(EQuantity::EMR);
	*out_radiance = sampler.sample(*m_radiance, X);
}

void BackgroundEmitter::genDirectSample(DirectLightSample& sample) const
{
	sample.pdfW = 0;
	sample.sourcePrim = m_surface;

	real uvSamplePdf;
	const math::Vector2R uvSample = m_sampleDistribution.sampleContinuous(
		math::Random::genUniformReal_i0_e1(),
		math::Random::genUniformReal_i0_e1(),
		&uvSamplePdf);

	m_surface->uvwToPosition(
		math::Vector3R(uvSample.x, uvSample.y, 0),
		sample.targetPos, 
		&(sample.emitPos));

	TSampler<SpectralStrength> sampler(EQuantity::EMR);
	sample.radianceLe = sampler.sample(*m_radiance, uvSample);
	
	// FIXME: assuming spherical uv mapping us used
	const real sinTheta = std::sin((1.0_r - uvSample.y) * math::constant::pi<real>);
	if(sinTheta <= 0.0_r)
	{
		return;
	}
	sample.pdfW = uvSamplePdf / (2.0_r * math::constant::pi<real> * math::constant::pi<real> * sinTheta);
}

// FIXME: ray time
void BackgroundEmitter::genSensingRay(Ray* out_ray, SpectralStrength* out_Le, math::Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const
{
	real uvSamplePdf;
	const math::Vector2R uvSample = m_sampleDistribution.sampleContinuous(
		math::Random::genUniformReal_i0_e1(),
		math::Random::genUniformReal_i0_e1(),
		&uvSamplePdf);

	TSampler<SpectralStrength> sampler(EQuantity::EMR);
	*out_Le = sampler.sample(*m_radiance, uvSample);

	// FIXME: assuming spherical uv mapping us used
	const real sinTheta = std::sin((1.0_r - uvSample.y) * math::constant::pi<real>);
	if(sinTheta <= 0.0_r)
	{
		return;
	}
	*out_pdfW = uvSamplePdf / (2.0_r * math::constant::pi<real> * math::constant::pi<real> * sinTheta);

	// HACK
	math::Vector3R direction;
	m_surface->uvwToPosition(
		math::Vector3R(uvSample.x, uvSample.y, 0),
		math::Vector3R(0, 0, 0),
		&direction);
	direction.normalizeLocal();
	direction.mulLocal(-1);
	*out_eN = direction;
	
	real diskPdf;
	math::Vector2R diskPos = math::UniformUnitDisk::map(
		{math::Random::genUniformReal_i0_e1(), math::Random::genUniformReal_i0_e1()},
		&diskPdf);

	*out_pdfA = diskPdf / (m_sceneBoundRadius * m_sceneBoundRadius);

	const auto basis = math::Basis3R::makeFromUnitY(direction);
	math::Vector3R position = direction.mul(-1) * m_sceneBoundRadius +
		(basis.getZAxis() * diskPos.x * m_sceneBoundRadius) +
		(basis.getXAxis() * diskPos.y * m_sceneBoundRadius);

	out_ray->setDirection(direction);
	out_ray->setOrigin(position);
	out_ray->setMinT(0.0001_r);// HACK: hard-code number
	out_ray->setMaxT(std::numeric_limits<real>::max());
}

real BackgroundEmitter::calcDirectSamplePdfW(
	const SurfaceHit&     emitPos, 
	const math::Vector3R& targetPos) const
{
	// FIXME: assuming spherical uv mapping us used
	const math::Vector3R uvw = emitPos.getDetail().getUvw();
	const real sinTheta = std::sin((1.0_r - uvw.y) * math::constant::pi<real>);
	if(sinTheta <= 0.0_r)
	{
		return 0.0_r;
	}

	return m_sampleDistribution.pdfContinuous({uvw.x, uvw.y}) / (2.0_r * math::constant::pi<real> * math::constant::pi<real> * sinTheta);
}

real BackgroundEmitter::calcRadiantFluxApprox() const
{
	PH_ASSERT(m_surface && m_radiance);

	return m_radiantFluxApprox;
}

}// end namespace ph
