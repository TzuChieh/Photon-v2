#include "Core/Emitter/SurfaceEmitter.h"
#include "Common/assertion.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/Quantity/ColorSpace.h"
#include "Math/math.h"
#include "Core/SurfaceHit.h"
#include "Core/HitDetail.h"
#include "Core/Intersectable/Primitive.h"

namespace ph
{

SurfaceEmitter::SurfaceEmitter() : 
	Emitter(),
	m_isBackFaceEmission(false)
{}

bool SurfaceEmitter::canEmit(const math::Vector3R& emitDirection, const math::Vector3R& N) const
{
	return math::is_same_hemisphere(emitDirection, N) != m_isBackFaceEmission;
}

void SurfaceEmitter::setFrontFaceEmit()
{
	m_isBackFaceEmission = false;
}

void SurfaceEmitter::setBackFaceEmit()
{
	m_isBackFaceEmission = true;
}

real SurfaceEmitter::calcPdfW(
	const Primitive* const emitSurface,
	const math::Vector3R&  emitPos,
	const math::Vector3R&  emitNormal,
	const math::Vector3R&  targetPos) const
{
	PH_ASSERT(emitSurface);

	math::Vector3R emitDir = targetPos.sub(emitPos);
	if(!canEmit(emitDir, emitNormal))
	{
		return 0.0_r;
	}
	emitDir.normalizeLocal();

	const real emitDirDotNormal = emitDir.dot(emitNormal);
	if(emitDirDotNormal == 0.0_r)
	{
		return 0.0_r;
	}

	const real samplePdfA  = emitSurface->calcPositionSamplePdfA(emitPos);
	const real distSquared = targetPos.sub(emitPos).lengthSquared();
	return samplePdfA / std::abs(emitDirDotNormal) * distSquared;
}

real SurfaceEmitter::calcPdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const
{
	return calcPdfW(
		emitPos.getDetail().getPrimitive(), 
		emitPos.getPosition(), 
		emitPos.getShadingNormal(), 
		targetPos);
}

}// end namespace ph
