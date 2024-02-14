#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"

#include <Common/assertion.h>

namespace ph
{

void Primitive::genPositionSample(PrimitivePosSampleQuery& query, SampleFlow& /* sampleFlow */) const
{
	query.setInvalidOutput();
	PH_ASSERT(!query.out);
}

}// end namespace ph
