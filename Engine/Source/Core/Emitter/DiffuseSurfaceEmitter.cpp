#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Math/TVector3.h"
#include "Actor/Geometry/Geometry.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/Intersectable/Primitive.h"
#include "Math/Random.h"
#include "Core/Sample/PositionSample.h"
#include "Core/Sample/DirectLightSample.h"
#include "Core/Ray.h"
#include "Math/constant.h"
#include "Core/Texture/TSampler.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Common/assertion.h"
#include "Core/Texture/SampleLocation.h"

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
	const Vector3R emitDir = X.getIncidentRay().getDirection().mul(-1.0_r);
	if(!canEmit(emitDir, X.getShadingNormal()))
	{
		out_radiance->setValues(0.0_r);
		return;
	}

	// TODO: able to specify channel or restrict it
	TSampler<SpectralStrength> sampler(EQuantity::EMR);
	*out_radiance = sampler.sample(getEmittedRadiance(), X);
}

void DiffuseSurfaceEmitter::genDirectSample(DirectLightSample& sample) const
{
	sample.pdfW = 0.0_r;
	sample.sourcePrim = m_surface;

	PositionSample positionSample;
	sample.sourcePrim->genPositionSample(&positionSample);
	if(positionSample.pdf == 0.0_r)
	{
		return;
	}

	const Vector3R emitterToTargetPos(sample.targetPos.sub(positionSample.position));
	const Vector3R emitDir(emitterToTargetPos.normalize());
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

real DiffuseSurfaceEmitter::calcDirectSamplePdfW(const SurfaceHit& emitPos, const Vector3R& targetPos) const
{
	return calcPdfW(emitPos, targetPos);
}

void DiffuseSurfaceEmitter::genSensingRay(Ray* const out_ray, SpectralStrength* const out_Le, Vector3R* const out_eN, real* const out_pdfA, real* const out_pdfW) const
{
	//// randomly and uniformly pick a primitive
	//const std::size_t picker = static_cast<std::size_t>(Random::genUniformReal_i0_e1() * static_cast<real>(m_primitives.size()));
	//const std::size_t pickedIndex = picker == m_primitives.size() ? picker - 1 : picker;
	//const Primitive* primitive = m_primitives[pickedIndex];
	//const real pickPdfW = (1.0_r / primitive->getReciExtendedArea()) * m_reciExtendedArea;

	//PositionSample positionSample;
	//primitive->genPositionSample(&positionSample);

	//PositionSample tPositionSample;
	//m_localToWorld->transformP(positionSample.position, &tPositionSample.position);
	//m_localToWorld->transformO(positionSample.normal, &tPositionSample.normal);
	//tPositionSample.normal.normalizeLocal();
	//tPositionSample.uvw = positionSample.uvw;
	//tPositionSample.pdf = positionSample.pdf;

	//// DEBUG
	////tPositionSample = positionSample;

	//// random & uniform direction on a unit sphere
	//Vector3R rayDir;
	//const real r1 = Random::genUniformReal_i0_e1();
	//const real r2 = Random::genUniformReal_i0_e1();
	//const real sqrtTerm = std::sqrt(r2 * (1.0_r - r2));
	//const real anglTerm = 2.0_r * PH_PI_REAL * r1;
	//rayDir.x = 2.0_r * std::cos(anglTerm) * sqrtTerm;
	//rayDir.y = 2.0_r * std::sin(anglTerm) * sqrtTerm;
	//rayDir.z = 1.0_r - 2.0_r * r2;
	//rayDir.normalizeLocal();

	//// TODO: time

	//out_ray->setDirection(rayDir);
	//out_ray->setOrigin(tPositionSample.position);
	//out_ray->setMinT(0.0001_r);// HACK: hard-code number
	//out_ray->setMaxT(Ray::MAX_T);
	//out_eN->set(tPositionSample.normal);
	//*out_pdfA = pickPdfW * tPositionSample.pdf;
	//*out_pdfW = 1.0_r / (4.0_r * PH_PI_REAL) / out_ray->getDirection().absDot(tPositionSample.normal);
	//m_emittedRadiance->sample(tPositionSample.uvw, out_Le);

	std::cerr << "PrimitiveAreaEmitter::genSensingRay() not implemented" << std::endl;
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

	PositionSample sample;
	m_surface->genPositionSample(&sample);
	if(sample.pdf == 0.0_r)
	{
		return SurfaceEmitter::calcRadiantFluxApprox();
	}

	// and assume the surface emits constant radiance sampled from that point

	constexpr EQuantity QUANTITY = EQuantity::EMR;

	const SpectralStrength sampledL = 
		TSampler<SpectralStrength>(QUANTITY).sample(*m_emittedRadiance, sample.uvw);
	SampledSpectralStrength spectralL;
	spectralL.setLinearSrgb(sampledL.genLinearSrgb(QUANTITY), QUANTITY);

	const real extendedArea = m_surface->calcExtendedArea();
	const real radiantFlux  = spectralL.sum() * extendedArea * PH_PI_REAL;
	return radiantFlux > 0.0_r ? radiantFlux : SurfaceEmitter::calcRadiantFluxApprox();
}

}// end namespace ph