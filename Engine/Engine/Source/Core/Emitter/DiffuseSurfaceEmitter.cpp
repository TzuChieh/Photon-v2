#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Math/TVector3.h"
#include "Actor/Geometry/Geometry.h"
#include "Core/Texture/constant_textures.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/Query/PrimitivePosSampleQuery.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Ray.h"
#include "Math/constant.h"
#include "Core/Texture/TSampler.h"
#include "Math/Color/Spectrum.h"
#include "Common/assertion.h"
#include "Core/Texture/SampleLocation.h"
#include "Math/Geometry/THemisphere.h"
#include "Math/TOrthonormalBasis3.h"
#include "Core/SampleGenerator/SampleFlow.h"
#include "Math/Color/spectral_samples.h"
#include "Math/Color/color_spaces.h"

#include <iostream>
#include <algorithm>
#include <cmath>

namespace ph
{

DiffuseSurfaceEmitter::DiffuseSurfaceEmitter(const Primitive* const surface) :
	SurfaceEmitter(), 
	m_surface(surface),
	m_emittedRadiance(nullptr)
{
	PH_ASSERT(surface);

	const real extendedArea = surface->calcExtendedArea();
	m_reciExtendedArea = extendedArea > 0.0_r ? 1.0_r / extendedArea : 0.0_r;

	static const auto DEFAULT_RADIANCE = math::Spectrum().setSpectral(
		math::resample_illuminant_D65<math::ColorValue>(), math::EColorUsage::EMR);

	setEmittedRadiance(std::make_shared<TConstantTexture<math::Spectrum>>(DEFAULT_RADIANCE));
}

void DiffuseSurfaceEmitter::evalEmittedRadiance(const SurfaceHit& X, math::Spectrum* const out_radiance) const
{
	const math::Vector3R emitDir = X.getIncidentRay().getDirection().mul(-1.0_r);
	if(!canEmit(emitDir, X.getShadingNormal()))
	{
		out_radiance->setColorValues(0.0_r);
		return;
	}

	// TODO: able to specify channel or restrict it
	TSampler<math::Spectrum> sampler(math::EColorUsage::EMR);
	*out_radiance = sampler.sample(getEmittedRadiance(), X);
}

void DiffuseSurfaceEmitter::genDirectSample(DirectEnergySampleQuery& query, SampleFlow& sampleFlow) const
{
	PrimitivePosSampleQuery positionSample;
	m_surface->genPositionSample(positionSample, sampleFlow);
	if(!positionSample.out)
	{
		return;
	}

	const math::Vector3R emitterToTargetPos(query.in.targetPos - positionSample.out.position);
	const math::Vector3R emitDir(emitterToTargetPos.normalize());
	if(!canEmit(emitDir, positionSample.out.normal))
	{
		return;
	}

	query.out.emitPos = positionSample.out.position;
	query.out.srcPrimitive = m_surface;

	const real distSquared = emitterToTargetPos.lengthSquared();
	const real emitDirDotNormal = emitDir.dot(positionSample.out.normal);
	query.out.pdfW = positionSample.out.pdfA / std::abs(emitDirDotNormal) * distSquared;

	// TODO: use sampler
	getEmittedRadiance().sample(
		SampleLocation(positionSample.out.uvw, math::EColorUsage::EMR), &query.out.radianceLe);
}

real DiffuseSurfaceEmitter::calcDirectSamplePdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const
{
	return calcPdfW(emitPos, targetPos);
}

void DiffuseSurfaceEmitter::emitRay(SampleFlow& sampleFlow, Ray* const out_ray, math::Spectrum* const out_Le, math::Vector3R* const out_eN, real* const out_pdfA, real* const out_pdfW) const
{
	PrimitivePosSampleQuery positionSample;
	m_surface->genPositionSample(positionSample, sampleFlow);

	/*real pdfW;
	Vector3R rayDir = UniformUnitHemisphere::map(
		{Random::genUniformReal_i0_e1(), Random::genUniformReal_i0_e1()},
		&pdfW);*/

	real pdfW;
	const auto localRayDir = math::THemisphere<real>::makeUnit().sampleToSurfaceCosThetaWeighted(
		sampleFlow.flow2D(),
		&pdfW);

	const auto sampleBasis = math::Basis3R::makeFromUnitY(positionSample.out.normal);
	math::Vector3R rayDir = sampleBasis.localToWorld(localRayDir);
	rayDir.normalizeLocal();

	// TODO: time

	out_ray->setDirection(rayDir);
	out_ray->setOrigin(positionSample.out.position);
	out_ray->setMinT(0.0001_r);// HACK: hard-code number
	out_ray->setMaxT(std::numeric_limits<real>::max());
	*out_eN = positionSample.out.normal;
	*out_pdfA = positionSample.out.pdfA;
	*out_pdfW = pdfW;
	*out_Le = TSampler<math::Spectrum>(math::EColorUsage::EMR).sample(*m_emittedRadiance, positionSample.out.uvw);
}

const Primitive* DiffuseSurfaceEmitter::getSurface() const
{
	return m_surface;
}

void DiffuseSurfaceEmitter::setEmittedRadiance(const std::shared_ptr<TTexture<math::Spectrum>>& emittedRadiance)
{
	PH_ASSERT(emittedRadiance);

	m_emittedRadiance = emittedRadiance;
}

const TTexture<math::Spectrum>& DiffuseSurfaceEmitter::getEmittedRadiance() const
{
	PH_ASSERT(m_emittedRadiance);

	return *m_emittedRadiance;
}

real DiffuseSurfaceEmitter::calcRadiantFluxApprox() const
{
	PH_ASSERT(m_emittedRadiance && m_surface);

	// randomly pick a point on the surface

	// TODO: more samples can be better
	PrimitivePosSampleQuery positionSample;
	SampleFlow randomFlow;
	m_surface->genPositionSample(positionSample, randomFlow);
	if(!positionSample.out)
	{
		return SurfaceEmitter::calcRadiantFluxApprox();
	}

	// FIXME: this assumption is too bad

	// and assume the surface emits constant radiance sampled from that point

	const auto sampledL = TSampler<math::Spectrum>(math::EColorUsage::EMR).sample(*m_emittedRadiance, positionSample.out.uvw);

	const real radiance     = math::estimate_color_energy<math::Spectrum::getColorSpace(), real>(sampledL.getColorValues());
	const real extendedArea = m_surface->calcExtendedArea();
	const real radiantFlux  = radiance * extendedArea * math::constant::pi<real>;

	return radiantFlux > 0.0_r ? radiantFlux : SurfaceEmitter::calcRadiantFluxApprox();
}

}// end namespace ph
