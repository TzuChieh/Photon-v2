#include "Core/Emitter/SurfaceEmitter.h"
#include "Math/math.h"
#include "Core/SurfaceHit.h"
#include "Core/HitDetail.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Emitter/Query/DirectEnergyPdfQuery.h"
#include "Core/Intersection/Query/PrimitivePosPdfQuery.h"
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

void SurfaceEmitter::calcDirectPdfWForSrcPrimitive(
	DirectEnergyPdfQuery& query,
	const lta::PDF& pickPdf,
	const lta::PDF& emitPosUvwPdf) const
{
	const auto emitterToTargetPos = query.inputs.getTargetPos() - query.inputs.getEmitPos();
	if(!query.inputs.getSrcPrimitive() || 
	   !canEmit(emitterToTargetPos, query.inputs.getEmitPosNormal()))
	{
		return;
	}

	PrimitivePosPdfQuery posPdf;
	posPdf.inputs.set(query.inputs, emitPosUvwPdf);
	query.inputs.getSrcPrimitive()->calcPosPdf(posPdf);
	if(!posPdf.outputs)
	{
		return;
	}

	PH_ASSERT(pickPdf.domain == lta::EDomain::Discrete);
	const real pdfW = lta::pdfA_to_pdfW(
		posPdf.outputs.getPdfA(), emitterToTargetPos, query.inputs.getEmitPosNormal());
	query.outputs.setPdf(lta::PDF::W(pdfW * pickPdf.value));
}

}// end namespace ph
