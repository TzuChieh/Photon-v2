#include "Core/Intersection/Query/PrimitivePosSamplePdfQuery.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/Emitter/Query/DirectEnergySamplePdfQuery.h"

namespace ph
{

void PrimitivePosSamplePdfInput::set(const PrimitivePosSampleOutput& sampleOutput, const SurfaceHit& X)
{
	PH_ASSERT(sampleOutput);

	set(sampleOutput.getPos(), sampleOutput.getObservationRay(), X.getDetail().getFaceID());
}

void PrimitivePosSamplePdfInput::set(const DirectEnergySamplePdfInput& pdfInput)
{
	set(pdfInput.getEmitPos(), pdfInput.getObservationRay(), pdfInput.getSrcFaceID());
}

}// end namespace ph
