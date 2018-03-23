#include "Core/Emitter/PrimitiveAreaEmitter.h"
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

PrimitiveAreaEmitter::PrimitiveAreaEmitter(const Primitive* const primitive) :
	Emitter(), 
	m_primitive(primitive),
	m_emittedRadiance(nullptr)
{
	PH_ASSERT(primitive != nullptr);

	SpectralStrength defaultSpectrum(1.0_r);
	m_emittedRadiance = std::make_shared<TConstantTexture<SpectralStrength>>(defaultSpectrum);

	m_reciExtendedArea = 1.0_r / primitive->calcExtendedArea();
}

PrimitiveAreaEmitter::~PrimitiveAreaEmitter() = default;

void PrimitiveAreaEmitter::evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* const out_radiance) const
{
	// FIXME: sort of hacked... (the direction of ray is reversed)
	// only front side of the emitter is emissive
	/*if(X.getIncidentRay().getDirection().mul(-1.0_r).dot(X.getShadingNormal()) <= 0.0_r)
	{
		out_radiance->setValues(0.0_r);
		return;
	}*/

	TSampler<SpectralStrength> sampler(EQuantity::EMR);
	*out_radiance = sampler.sample(*m_emittedRadiance, X);
}

void PrimitiveAreaEmitter::genDirectSample(DirectLightSample& sample) const
{
	sample.sourcePrim = m_primitive;

	PositionSample positionSample;
	sample.sourcePrim->genPositionSample(&positionSample);

	const Vector3R emitterToTargetPos(sample.targetPos.sub(positionSample.position));
	const Vector3R emitDir(emitterToTargetPos.normalize());
	const real distSquared = emitterToTargetPos.lengthSquared();
	
	sample.emitPos = positionSample.position;

	const real emitDirDotNormal = emitDir.dot(positionSample.normal);
	if(emitDirDotNormal <= 0.0_r)
	{
		sample.pdfW = 0.0_r;
		return;
	}
	sample.pdfW = positionSample.pdf / std::abs(emitDirDotNormal) * distSquared;
	m_emittedRadiance->sample(SampleLocation(positionSample.uvw, EQuantity::EMR), &sample.radianceLe);

	/*std::cerr << sample.emitPos.toString() << std::endl;
	std::cerr << sample.radianceLe.toString() << std::endl;
	std::cerr << sample.pdfW << std::endl;*/
}

real PrimitiveAreaEmitter::calcDirectSamplePdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Primitive* hitPrim) const
{
	PH_ASSERT(hitPrim != nullptr);

	const Vector3R emitDir(targetPos.sub(emitPos).normalizeLocal());
	const real emitDirDotNormal = emitDir.dot(emitN);
	if(emitDirDotNormal <= 0.0_r)
	{
		return 0.0_r;
	}

	const real samplePdfA  = hitPrim->calcPositionSamplePdfA(emitPos);
	const real distSquared = targetPos.sub(emitPos).lengthSquared();
	return samplePdfA / std::abs(emitDirDotNormal) * distSquared;
}

void PrimitiveAreaEmitter::genSensingRay(Ray* const out_ray, SpectralStrength* const out_Le, Vector3R* const out_eN, real* const out_pdfA, real* const out_pdfW) const
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

void PrimitiveAreaEmitter::setEmittedRadiance(
	const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance)
{
	m_emittedRadiance = emittedRadiance;
}

const Primitive* PrimitiveAreaEmitter::getPrimitive() const
{
	return m_primitive;
}

}// end namespace ph