#include "Core/Intersectable/Primitive.h"
#include "Common/assertion.h"
#include "Core/Intersectable/Query/PrimitivePosSampleQuery.h"

namespace ph
{

void Primitive::genPositionSample(PrimitivePosSampleQuery& query, SampleFlow& /* sampleFlow */) const
{
	query.setInvalidOutput();
	PH_ASSERT(!query.out);
}

}// end namespace ph
