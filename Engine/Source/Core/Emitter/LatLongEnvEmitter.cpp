#include "Core/Emitter/LatLongEnvEmitter.h"
#include "Common/assertion.h"
#include "Core/SurfaceHit.h"
#include "Core/Texture/TSampler.h"
#include "Common/Logger.h"
#include "Core/Intersectable/UvwMapper/SphericalMapper.h"
#include "Core/Sample/DirectLightSample.h"
#include "Core/Intersectable/PLatLongEnvSphere.h"
#include "Math/constant.h"
#include "Math/math.h"
#include "Math/Geometry/TDisk.h"
#include "Math/TOrthonormalBasis3.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <vector>
#include <cmath>
#include <algorithm>

namespace ph
{

namespace
{
	const Logger logger(LogSender("Background Emitter"));
}

LatLongEnvEmitter::LatLongEnvEmitter(
	const PLatLongEnvSphere* const surface,
	const RadianceTexture&         radiance,
	const math::Vector2S&          resolution) :

	m_surface           (surface),
	m_radiance          (radiance),
	m_sampleDistribution(),
	m_radiantFluxApprox (0)
{
	PH_ASSERT(surface);
	PH_ASSERT(radiance);
	PH_ASSERT_GT(resolution.x * resolution.y, 0);

	logger.log(ELogLevel::NOTE_MED, 
		"constructing sample distribution with resolution " + resolution.toString());

	constexpr EQuantity QUANTITY = EQuantity::EMR;
	const real rcpResolutionY = 1.0_r / static_cast<real>(resolution.y);
	const TSampler<Spectrum> sampler(QUANTITY);

	std::vector<real> sampleWeights(resolution.x * resolution.y);
	for(std::size_t y = 0; y < resolution.y; ++y)
	{
		const std::size_t baseIndex = y * resolution.x;
		const real        v         = (static_cast<real>(y) + 0.5_r) * rcpResolutionY;
		const real        sinTheta  = std::sin((1.0_r - v) * math::constant::pi<real>);
		for(std::size_t x = 0; x < resolution.x; ++x)
		{
			const real     u        = (static_cast<real>(x) + 0.5_r) / static_cast<real>(resolution.x);
			const Spectrum sampledL = sampler.sample(*radiance, {u, v});

			// For non-nearest filtered textures, sample weights can be 0 while
			// there is still energy around that point (because its neighbor 
			// may have non-zero energy), ensure a lower bound to avoid this
			constexpr real MIN_LUMINANCE = 1e-6;
			const real luminance = std::max(sampledL.calcLuminance(QUANTITY), MIN_LUMINANCE);

			// FIXME: using different PDF resolution can under sample the texture
			// use mipmaps perhaps?
			sampleWeights[baseIndex + x] = luminance * sinTheta;

			m_radiantFluxApprox += sampleWeights[baseIndex + x];
		}
	}

	m_sampleDistribution = math::TPwcDistribution2D<real>(sampleWeights.data(), resolution);
	m_radiantFluxApprox  = m_radiantFluxApprox * m_surface->calcExtendedArea();
}

void LatLongEnvEmitter::evalEmittedRadiance(
	const SurfaceHit& X, 
	Spectrum* const   out_radiance) const
{
	PH_ASSERT(out_radiance);
	PH_ASSERT(m_radiance);

	TSampler<Spectrum> sampler(EQuantity::EMR);
	*out_radiance = sampler.sample(*m_radiance, X);
}

void LatLongEnvEmitter::genDirectSample(SampleFlow& sampleFlow, DirectLightSample& sample) const
{
	sample.pdfW = 0;
	sample.sourcePrim = m_surface;

	real uvSamplePdf;
	const math::Vector2R uvSample = m_sampleDistribution.sampleContinuous(
		sampleFlow.flow2D(),
		&uvSamplePdf);

	if(!m_surface->latLong01ToSurface(uvSample, sample.targetPos, &(sample.emitPos)))
	{
		return;
	}

	TSampler<Spectrum> sampler(EQuantity::EMR);
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
void LatLongEnvEmitter::emitRay(SampleFlow& sampleFlow, Ray* out_ray, Spectrum* out_Le, math::Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const
{
	real uvSamplePdf;
	const math::Vector2R uvSample = m_sampleDistribution.sampleContinuous(
		sampleFlow.flow2D(),
		&uvSamplePdf);

	TSampler<Spectrum> sampler(EQuantity::EMR);
	*out_Le = sampler.sample(*m_radiance, uvSample);

	const real sinTheta = std::sin((1.0_r - uvSample.y) * math::constant::pi<real>);
	*out_pdfW = uvSamplePdf / (2.0_r * math::constant::pi<real> * math::constant::pi<real> * sinTheta);

	// HACK
	math::Vector3R direction;
	m_surface->latLong01ToSurface(
		uvSample,
		math::Vector3R(0, 0, 0),
		&direction);
	direction.normalizeLocal();
	direction.mulLocal(-1);
	*out_eN = direction;
	
	real diskPdf;
	math::Vector2R diskPos = math::TDisk<real>(1.0_r).sampleToSurface2D(
		sampleFlow.flow2D(), &diskPdf);

	*out_pdfA = diskPdf / (m_surface->getRadius() * m_surface->getRadius());

	const auto basis = math::Basis3R::makeFromUnitY(direction);
	math::Vector3R position = direction.mul(-1) * m_surface->getRadius() +
		(basis.getZAxis() * diskPos.x * m_surface->getRadius()) +
		(basis.getXAxis() * diskPos.y * m_surface->getRadius());// TODO: use TDisk to do this

	out_ray->setDirection(direction);
	out_ray->setOrigin(position);
	out_ray->setMinT(0.0001_r);// HACK: hard-code number
	out_ray->setMaxT(std::numeric_limits<real>::max());
}

real LatLongEnvEmitter::calcDirectSamplePdfW(
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

real LatLongEnvEmitter::calcRadiantFluxApprox() const
{
	PH_ASSERT(m_surface && m_radiance);

	return m_radiantFluxApprox;
}

}// end namespace ph
