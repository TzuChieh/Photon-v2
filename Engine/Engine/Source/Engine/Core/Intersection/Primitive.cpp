#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Engine/Core/Intersection/Query/PrimitivePosPdfQuery.h"
#include "Engine/World/SceneGlobals.h"

#include <Common/assertion.h>

namespace ph
{

void Primitive::genPosSample(
	PrimitivePosSampleQuery& query,
	SampleFlow& /* sampleFlow */) const
{
	query.outputs.setPdfPos({});
	PH_ASSERT(!query.outputs.isContributable());
}

void Primitive::calcPosPdf(PrimitivePosPdfQuery& query) const
{
	query.outputs.setPdf({});
	PH_ASSERT(!query.outputs);
}

const PrimitiveMetadata& Primitive::getMetadata(uint32 /* slot */) const
{
	return *SceneGlobals::getDefaultPrimitiveMetadata();
}

}// end namespace ph
