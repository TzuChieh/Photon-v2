#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/Intersection/Query/PrimitivePosPdfQuery.h"

#include <Common/assertion.h>

namespace ph
{

void Primitive::genPosSample(
	PrimitivePosSampleQuery& query,
	SampleFlow& /* sampleFlow */,
	HitProbe& /* probe */) const
{
	query.outputs = {};
	PH_ASSERT(!query.outputs);
}

void Primitive::calcPosPdf(PrimitivePosPdfQuery& query) const
{
	query.outputs = {};
	PH_ASSERT(!query.outputs);
}

}// end namespace ph
