#include "Core/Intersectable/Primitive.h"
#include "Common/assertion.h"
#include "Core/Intersectable/Query/PrimitivePosSampleQuery.h"

#include <iostream>

namespace ph
{

Primitive::Primitive(const PrimitiveMetadata* const metadata) :
	m_metadata(metadata)
{
	PH_ASSERT(metadata);
}

void Primitive::genPositionSample(PrimitivePosSampleQuery& query, SampleFlow& /* sampleFlow */) const
{
	query.outputs.pdfA = 0;
	PH_ASSERT(!query.outputs);
}

}// end namespace ph
