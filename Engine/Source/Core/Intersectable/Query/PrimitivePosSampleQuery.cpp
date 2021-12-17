#include "Core/Intersectable/Query/PrimitivePosSampleQuery.h"
#include "Core/HitDetail.h"

namespace ph
{

PrimitivePosSampleOutput::PrimitivePosSampleOutput() :
	position(0),
	normal  (0, 1, 0),
	uvw     (0),
	faceId  (HitDetail::NO_FACE_ID),
	pdfA    (0)
{
	PH_ASSERT(!(*this));
}

}// end namespace ph
