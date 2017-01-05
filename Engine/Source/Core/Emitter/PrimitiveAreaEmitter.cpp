#include "Core/Emitter/PrimitiveAreaEmitter.h"
#include "Math/Vector3f.h"
#include "Actor/Geometry/Geometry.h"
#include "Core/Intersection.h"
#include "Image/ConstantTexture.h"
#include "Core/Primitive/Primitive.h"
#include "Math/random_number.h"
#include "Core/Sample/PositionSample.h"
#include "Core/Sample/DirectLightSample.h"

#include <iostream>

namespace ph
{

PrimitiveAreaEmitter::PrimitiveAreaEmitter(const std::vector<const Primitive*>& primitives) :
	Emitter(), 
	m_primitives(primitives), m_emittedRadiance(std::make_shared<ConstantTexture>(Vector3f(0, 0, 0)))
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
	m_reciExtendedArea = static_cast<float32>(1.0 / extendedArea);
}

PrimitiveAreaEmitter::~PrimitiveAreaEmitter()
{

}

void PrimitiveAreaEmitter::evalEmittedRadiance(const Intersection& intersection, Vector3f* const out_emitterRadiance) const
{
	m_emittedRadiance->sample(intersection.getHitUVW(), out_emitterRadiance);
}

void PrimitiveAreaEmitter::genDirectSample(const Vector3f& targetPos, Vector3f* const out_emitPos, Vector3f* const out_emittedRadiance, float32* const out_PDF) const
{
	const std::size_t picker = static_cast<std::size_t>(genRandomFloat32_0_1_uniform() * static_cast<float32>(m_primitives.size()));
	const std::size_t pickedIndex = picker == m_primitives.size() ? picker - 1 : picker;

	const Primitive* primitive = m_primitives[pickedIndex];
	PositionSample positionSample;
	primitive->genPositionSample(&positionSample);

	const float32 distSquared = targetPos.sub(positionSample.position).squaredLength();
	const Vector3f emitDir = targetPos.sub(positionSample.position).normalizeLocal();
	const float32 pickPDF = (1.0f / primitive->getReciExtendedArea()) * m_reciExtendedArea;
	//*out_PDF = pickPDF * positionSample.pdf / (std::abs(emitDir.dot(positionSample.normal)) / distSquared);
	*out_PDF = pickPDF * positionSample.pdf / std::abs(emitDir.dot(positionSample.normal)) * distSquared;

	/*if(*out_PDF < 0.0f)
	{
		*out_PDF = 0.0f;
	}*/

	Vector3f emittedRadiance;
	m_emittedRadiance->sample(positionSample.uvw, &emittedRadiance);
	*out_emittedRadiance = emittedRadiance;

	*out_emitPos = positionSample.position;
}

void PrimitiveAreaEmitter::genDirectSample(DirectLightSample& sample) const
{
	// randomly and uniformly pick a primitive
	const std::size_t picker = static_cast<std::size_t>(genRandomFloat32_0_1_uniform() * static_cast<float32>(m_primitives.size()));
	const std::size_t pickedIndex = picker == m_primitives.size() ? picker - 1 : picker;
	sample.sourcePrim = m_primitives[pickedIndex];
	const float32 pickPdfW = (1.0f / sample.sourcePrim->getReciExtendedArea()) * m_reciExtendedArea;

	PositionSample positionSample;
	sample.sourcePrim->genPositionSample(&positionSample);

	const Vector3f emitterToTargetPos(sample.targetPos.sub(positionSample.position));
	const Vector3f emitDir(emitterToTargetPos.normalize());
	const float32 distSquared = emitterToTargetPos.squaredLength();
	
	sample.emitPos = positionSample.position;
	sample.pdfW = pickPdfW * positionSample.pdf / std::abs(emitDir.dot(positionSample.normal)) * distSquared;
	m_emittedRadiance->sample(positionSample.uvw, &sample.radianceLe);
}

float32 PrimitiveAreaEmitter::calcDirectSamplePdfW(const Vector3f& targetPos, const Vector3f& emitPos, const Vector3f& emitN, const Primitive* hitPrim) const
{
	const float32 pickPdfW = (1.0f / hitPrim->getReciExtendedArea()) * m_reciExtendedArea;
	const float32 samplePdfA = hitPrim->calcPositionSamplePdfA(emitPos);
	const float32 distSquared = targetPos.sub(emitPos).squaredLength();
	const Vector3f emitDir(targetPos.sub(emitPos).normalizeLocal());
	return pickPdfW * (samplePdfA / std::abs(emitDir.dot(emitN)) * distSquared);
}

void PrimitiveAreaEmitter::setEmittedRadiance(const std::shared_ptr<Texture> emittedRadiance)
{
	m_emittedRadiance = emittedRadiance;
}

}// end namespace ph