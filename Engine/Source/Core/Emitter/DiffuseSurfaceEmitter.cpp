#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Math/TVector3.h"
#include "Actor/Geometry/Geometry.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Sample/PositionSample.h"
#include "Core/Sample/DirectLightSample.h"
#include "Core/Ray.h"
#include "Math/constant.h"
#include "Core/Texture/TSampler.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Common/assertion.h"
#include "Core/Texture/SampleLocation.h"
#include "Math/Geometry/THemisphere.h"
#include "Math/TOrthonormalBasis3.h"
#include "Core/SampleGenerator/SampleFlow.h"

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

	SpectralStrength defaultRadiance;
	defaultRadiance.setSampled(ColorSpace::get_D65_SPD());
	setEmittedRadiance(std::make_shared<TConstantTexture<SpectralStrength>>(defaultRadiance));
}

void DiffuseSurfaceEmitter::evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* const out_radiance) const
{
	const math::Vector3R emitDir = X.getIncidentRay().getDirection().mul(-1.0_r);
	if(!canEmit(emitDir, X.getShadingNormal()))
	{
		out_radiance->setValues(0.0_r);
		return;
	}

	// TODO: able to specify channel or restrict it
	TSampler<SpectralStrength> sampler(EQuantity::EMR);
	*out_radiance = sampler.sample(getEmittedRadiance(), X);
}

void DiffuseSurfaceEmitter::genDirectSample(SampleFlow& sampleFlow, DirectLightSample& sample) const
{
	sample.pdfW = 0.0_r;
	sample.sourcePrim = m_surface;

	PositionSample positionSample;
	sample.sourcePrim->genPositionSample(sampleFlow, &positionSample);
	if(positionSample.pdf == 0.0_r)
	{
		return;
	}

	const math::Vector3R emitterToTargetPos(sample.targetPos.sub(positionSample.position));
	const math::Vector3R emitDir(emitterToTargetPos.normalize());
	const real distSquared = emitterToTargetPos.lengthSquared();
	
	sample.emitPos = positionSample.position;

	if(!canEmit(emitDir, positionSample.normal))
	{
		return;
	}

	const real emitDirDotNormal = emitDir.dot(positionSample.normal);
	sample.pdfW = positionSample.pdf / std::abs(emitDirDotNormal) * distSquared;

	// TODO: use sampler
	getEmittedRadiance().sample(SampleLocation(positionSample.uvw, EQuantity::EMR), &sample.radianceLe);
}

real DiffuseSurfaceEmitter::calcDirectSamplePdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const
{
	return calcPdfW(emitPos, targetPos);
}

void DiffuseSurfaceEmitter::emitRay(SampleFlow& sampleFlow, Ray* const out_ray, SpectralStrength* const out_Le, math::Vector3R* const out_eN, real* const out_pdfA, real* const out_pdfW) const
{
	PositionSample positionSample;
	m_surface->genPositionSample(sampleFlow, &positionSample);

	/*real pdfW;
	Vector3R rayDir = UniformUnitHemisphere::map(
		{Random::genUniformReal_i0_e1(), Random::genUniformReal_i0_e1()},
		&pdfW);*/

	real pdfW;
	const auto localRayDir = math::THemisphere<real>::makeUnit().sampleToSurfaceCosThetaWeighted(
		sampleFlow.flow2D(),
		&pdfW);

	const auto sampleBasis = math::Basis3R::makeFromUnitY(positionSample.normal);
	math::Vector3R rayDir = sampleBasis.localToWorld(localRayDir);
	rayDir.normalizeLocal();

	// TODO: time

	out_ray->setDirection(rayDir);
	out_ray->setOrigin(positionSample.position);
	out_ray->setMinT(0.0001_r);// HACK: hard-code number
	out_ray->setMaxT(std::numeric_limits<real>::max());
	out_eN->set(positionSample.normal);
	*out_pdfA = positionSample.pdf;
	*out_pdfW = pdfW;
	*out_Le = TSampler<SpectralStrength>(EQuantity::EMR).sample(*m_emittedRadiance, positionSample.uvw);
}

const Primitive* DiffuseSurfaceEmitter::getSurface() const
{
	return m_surface;
}

void DiffuseSurfaceEmitter::setEmittedRadiance(const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance)
{
	PH_ASSERT(emittedRadiance);

	m_emittedRadiance = emittedRadiance;
}

const TTexture<SpectralStrength>& DiffuseSurfaceEmitter::getEmittedRadiance() const
{
	PH_ASSERT(m_emittedRadiance);

	return *m_emittedRadiance;
}

real DiffuseSurfaceEmitter::calcRadiantFluxApprox() const
{
	PH_ASSERT(m_emittedRadiance && m_surface);

	// randomly pick a point on the surface

	// TODO: more samples can be better
	PositionSample sample;
	SampleFlow randomFlow;
	m_surface->genPositionSample(randomFlow, &sample);
	if(sample.pdf == 0.0_r)
	{
		return SurfaceEmitter::calcRadiantFluxApprox();
	}

	// FIXME: this assumption is too bad

	// and assume the surface emits constant radiance sampled from that point

	constexpr EQuantity QUANTITY = EQuantity::EMR;

	const SpectralStrength sampledL = 
		TSampler<SpectralStrength>(QUANTITY).sample(*m_emittedRadiance, sample.uvw);
	SampledSpectralStrength spectralL;
	spectralL.setLinearSrgb(sampledL.genLinearSrgb(QUANTITY), QUANTITY);

	const real extendedArea = m_surface->calcExtendedArea();
	const real radiantFlux  = spectralL.sum() * extendedArea * math::constant::pi<real>;
	return radiantFlux > 0.0_r ? radiantFlux : SurfaceEmitter::calcRadiantFluxApprox();
}

}// end namespace ph
