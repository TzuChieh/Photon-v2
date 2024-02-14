#include "Core/Intersection/Query/PrimitivePosSamplePdfQuery.h"
#include "Core/HitDetail.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"

#include <Common/assertion.h>

namespace ph
{

PrimitivePosSamplePdfInput::PrimitivePosSamplePdfInput() :
	position(0),
	faceId  (HitDetail::NO_FACE_ID)
{}

void PrimitivePosSamplePdfInput::set(const math::Vector3R& position)
{
	set(position, HitDetail::NO_FACE_ID);
}

void PrimitivePosSamplePdfInput::set(const math::Vector3R& position, const uint64 faceId)
{
	this->position = position;
	this->faceId   = faceId;
}

void PrimitivePosSamplePdfInput::set(const HitDetail& detail)
{
	set(detail.getPosition(), detail.getFaceId());
}

void PrimitivePosSamplePdfInput::set(const PrimitivePosSampleQuery& query)
{
	PH_ASSERT(query.out);

	set(query.out.position, query.out.faceId);
}

}// end namespace ph
