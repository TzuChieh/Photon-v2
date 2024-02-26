#include "Core/Emitter/SurfaceEmitter.h"
#include "Math/math.h"
#include "Core/SurfaceHit.h"
#include "Core/HitDetail.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Emitter/Query/DirectEnergySamplePdfQuery.h"
#include "Core/Intersection/Query/PrimitivePosSamplePdfQuery.h"
#include "Core/LTA/lta.h"

#include <Common/assertion.h>

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

void SurfaceEmitter::calcDirectSamplePdfWForSingleSurface(
	DirectEnergySamplePdfQuery& query,
	HitProbe& probe) const
{
	query.outputs.setPdfW(0);

	const auto emitterToTargetPos = query.inputs.getTargetPos() - query.inputs.getEmitPos();
	if(!canEmit(emitterToTargetPos, query.inputs.getEmitPosNormal()))
	{
		return;
	}

	PrimitivePosSamplePdfQuery posSample;
	posSample.inputs.set(query.inputs);

	PH_ASSERT(query.inputs.getSrcPrimitive());
	query.inputs.getSrcPrimitive()->calcPosSamplePdfA(posSample, probe);
	if(!posSample.outputs)
	{
		return;
	}

	query.outputs.setPdfW(lta::pdfA_to_pdfW(
		posSample.outputs.getPdfA(), emitterToTargetPos, query.inputs.getEmitPosNormal()));
}

}// end namespace ph
