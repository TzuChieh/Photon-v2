#include "Core/Emitter/PrimitiveAreaEmitter.h"
#include "Math/TVector3.h"
#include "Actor/Geometry/Geometry.h"
#include "Core/Intersection.h"
#include "Actor/Texture/ConstantTexture.h"
#include "Core/Intersectable/Primitive.h"
#include "Math/Random.h"
#include "Core/Sample/PositionSample.h"
#include "Core/Sample/DirectLightSample.h"
#include "Core/Ray.h"
#include "Math/constant.h"

#include <iostream>
#include <algorithm>
#include <cmath>

namespace ph
{

PrimitiveAreaEmitter::PrimitiveAreaEmitter(const std::vector<const Primitive*>& primitives) :
	Emitter(), 
	m_primitives(primitives), m_emittedRadiance(std::make_shared<ConstantTexture>(Vector3R(0, 0, 0)))
{
	if(primitives.empty())
	{
		std::cerr << "warning: at PrimitiveAreaEmitter::PrimitiveAreaEmitter(), no Primitive detected" << std::endl;
	}

	float64 extendedArea = 0.0;
	for(const auto& primitive : primitives)
	{
		extendedArea += 1.0f / primitive->getReciExtendedArea();
	}
	m_reciExtendedArea = static_cast<real>(1.0 / extendedArea);
}

PrimitiveAreaEmitter::~PrimitiveAreaEmitter()
{

}

void PrimitiveAreaEmitter::evalEmittedRadiance(const Intersection& intersection, Vector3R* const out_emitterRadiance) const
{
	m_emittedRadiance->sample(intersection.getHitUVW(), out_emitterRadiance);
}

void PrimitiveAreaEmitter::genDirectSample(const Vector3R& targetPos, Vector3R* const out_emitPos, Vector3R* const out_emittedRadiance, real* const out_PDF) const
{
	const std::size_t picker = static_cast<std::size_t>(Random::genUniformReal_i0_e1() * static_cast<real>(m_primitives.size()));
	const std::size_t pickedIndex = picker == m_primitives.size() ? picker - 1 : picker;

	const Primitive* primitive = m_primitives[pickedIndex];
	PositionSample positionSample;
	primitive->genPositionSample(&positionSample);

	PositionSample tPositionSample;
	m_localToWorld->transformP(positionSample.position, &tPositionSample.position);
	m_localToWorld->transformO(positionSample.normal, &tPositionSample.normal);
	tPositionSample.normal.normalizeLocal();
	tPositionSample.uvw = positionSample.uvw;
	tPositionSample.pdf = positionSample.pdf;

	// DEBUG
	//tPositionSample = positionSample;

	const real distSquared = targetPos.sub(tPositionSample.position).lengthSquared();
	const Vector3R emitDir = targetPos.sub(tPositionSample.position).normalizeLocal();
	const real pickPDF = (1.0_r / primitive->getReciExtendedArea()) * m_reciExtendedArea;
	//*out_PDF = pickPDF * positionSample.pdf / (std::abs(emitDir.dot(positionSample.normal)) / distSquared);
	*out_PDF = pickPDF * tPositionSample.pdf / std::abs(emitDir.dot(tPositionSample.normal)) * distSquared;

	/*if(*out_PDF < 0.0f)
	{
		*out_PDF = 0.0f;
	}*/

	Vector3R emittedRadiance;
	m_emittedRadiance->sample(tPositionSample.uvw, &emittedRadiance);
	*out_emittedRadiance = emittedRadiance;

	*out_emitPos = tPositionSample.position;
}

void PrimitiveAreaEmitter::genDirectSample(DirectLightSample& sample) const
{
	// randomly and uniformly pick a primitive
	const std::size_t picker = static_cast<std::size_t>(Random::genUniformReal_i0_e1() * static_cast<real>(m_primitives.size()));
	const std::size_t pickedIndex = picker == m_primitives.size() ? picker - 1 : picker;
	sample.sourcePrim = m_primitives[pickedIndex];
	const real pickPdfW = (1.0_r / sample.sourcePrim->getReciExtendedArea()) * m_reciExtendedArea;

	PositionSample positionSample;
	sample.sourcePrim->genPositionSample(&positionSample);

	PositionSample tPositionSample;
	m_localToWorld->transformP(positionSample.position, &tPositionSample.position);
	m_localToWorld->transformO(positionSample.normal, &tPositionSample.normal);
	tPositionSample.normal.normalizeLocal();
	tPositionSample.uvw = positionSample.uvw;
	tPositionSample.pdf = positionSample.pdf;

	// DEBUG
	//tPositionSample = positionSample;

	const Vector3R emitterToTargetPos(sample.targetPos.sub(tPositionSample.position));
	const Vector3R emitDir(emitterToTargetPos.normalize());
	const real distSquared = emitterToTargetPos.lengthSquared();
	
	sample.emitPos = tPositionSample.position;
	sample.pdfW = pickPdfW * tPositionSample.pdf / std::abs(emitDir.dot(tPositionSample.normal)) * distSquared;
	m_emittedRadiance->sample(tPositionSample.uvw, &sample.radianceLe);
}

real PrimitiveAreaEmitter::calcDirectSamplePdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Primitive* hitPrim) const
{
	const real pickPdfW = (1.0_r / hitPrim->getReciExtendedArea()) * m_reciExtendedArea;
	const real samplePdfA = hitPrim->calcPositionSamplePdfA(emitPos);
	const real distSquared = targetPos.sub(emitPos).lengthSquared();
	const Vector3R emitDir(targetPos.sub(emitPos).normalizeLocal());
	return pickPdfW * (samplePdfA / std::abs(emitDir.dot(emitN)) * distSquared);
}

void PrimitiveAreaEmitter::genSensingRay(Ray* const out_ray, Vector3R* const out_Le, Vector3R* const out_eN, real* const out_pdfA, real* const out_pdfW) const
{
	// randomly and uniformly pick a primitive
	const std::size_t picker = static_cast<std::size_t>(Random::genUniformReal_i0_e1() * static_cast<real>(m_primitives.size()));
	const std::size_t pickedIndex = picker == m_primitives.size() ? picker - 1 : picker;
	const Primitive* primitive = m_primitives[pickedIndex];
	const real pickPdfW = (1.0_r / primitive->getReciExtendedArea()) * m_reciExtendedArea;

	PositionSample positionSample;
	primitive->genPositionSample(&positionSample);

	PositionSample tPositionSample;
	m_localToWorld->transformP(positionSample.position, &tPositionSample.position);
	m_localToWorld->transformO(positionSample.normal, &tPositionSample.normal);
	tPositionSample.normal.normalizeLocal();
	tPositionSample.uvw = positionSample.uvw;
	tPositionSample.pdf = positionSample.pdf;

	// DEBUG
	//tPositionSample = positionSample;

	// random & uniform direction on a unit sphere
	Vector3R rayDir;
	const real r1 = Random::genUniformReal_i0_e1();
	const real r2 = Random::genUniformReal_i0_e1();
	const real sqrtTerm = std::sqrt(r2 * (1.0_r - r2));
	const real anglTerm = 2.0_r * PI_REAL * r1;
	rayDir.x = 2.0_r * std::cos(anglTerm) * sqrtTerm;
	rayDir.y = 2.0_r * std::sin(anglTerm) * sqrtTerm;
	rayDir.z = 1.0_r - 2.0_r * r2;
	rayDir.normalizeLocal();

	// TODO: time

	out_ray->setDirection(rayDir);
	out_ray->setOrigin(tPositionSample.position);
	out_ray->setMinT(0.0001_r);// HACK: hard-code number
	out_ray->setMaxT(Ray::MAX_T);
	out_eN->set(tPositionSample.normal);
	*out_pdfA = pickPdfW * tPositionSample.pdf;
	*out_pdfW = 1.0_r / (4.0_r * PI_REAL) / out_ray->getDirection().absDot(tPositionSample.normal);
	m_emittedRadiance->sample(tPositionSample.uvw, out_Le);
}

void PrimitiveAreaEmitter::setEmittedRadiance(const std::shared_ptr<Texture>& emittedRadiance)
{
	m_emittedRadiance = emittedRadiance;
}

}// end namespace ph