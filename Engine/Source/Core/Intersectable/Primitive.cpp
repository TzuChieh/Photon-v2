#include "Core/Intersectable/Primitive.h"
#include "Common/assertion.h"
#include "Core/Intersectable/Query/PrimitivePosSampleQuery.h"

namespace ph
{

void Primitive::genPositionSample(PrimitivePosSampleQuery& query, SampleFlow& /* sampleFlow */) const
{
	query.out.pdfA = 0;
	PH_ASSERT(!query.out);
}

}// end namespace ph
