#include "Engine/Core/Emitter/SurfaceEmitter.h"
#include "Engine/Math/math.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Emitter/Query/DirectEnergyPdfQuery.h"
#include "Engine/Core/Intersection/Query/PrimitivePosPdfQuery.h"
#include "Engine/Core/LTA/lta.h"

#include <Common/assertion.h>

namespace ph
{

SurfaceEmitter::SurfaceEmitter(const EmitterFeatureSet featureSet)
	: Emitter(featureSet)
	, m_isBackFaceEmission(false)
{}

bool SurfaceEmitter::canEmit(const math::Vector3R& emitDir, const math::Vector3R& N) const
{
	return m_isBackFaceEmission
		? math::is_opposite_hemisphere(emitDir, N)
		: math::is_same_hemisphere(emitDir, N);
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
	if(getFeatureSet().hasNo(EEmitterFeatureSet::DirectSample) ||
	   !canEmit(emitterToTargetPos, query.inputs.getEmitPosNormal()))
	{
		return;
	}

	PrimitivePosPdfQuery posPdf;
	posPdf.inputs.set(query.inputs, emitPosUvwPdf);
	query.inputs.getSrcPrimitive().calcPosPdf(posPdf);
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
