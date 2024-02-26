#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/Intersection/Query/PrimitivePosSamplePdfQuery.h"

#include <Common/assertion.h>

namespace ph
{

void Primitive::genPosSample(
	PrimitivePosSampleQuery& query,
	SampleFlow& /* sampleFlow */,
	HitProbe& /* probe */) const
{
	query.outputs.invalidate();
	PH_ASSERT(!query.outputs);
}

void Primitive::calcPosSamplePdfA(
	PrimitivePosSamplePdfQuery& query,
	HitProbe& probe) const
{
	query.outputs.setPdfA(0);
	PH_ASSERT(!query.outputs);
}

}// end namespace ph
