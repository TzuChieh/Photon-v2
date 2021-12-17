#include "Core/Intersectable/Query/PrimitivePosSamplePdfQuery.h"
#include "Core/HitDetail.h"
#include "Core/Intersectable/Query/PrimitivePosSampleQuery.h"
#include "Common/assertion.h"

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

void PrimitivePosSamplePdfInput::set(const math::Vector3R& position, const std::size_t faceId)
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
	PH_ASSERT(query.outputs);

	set(query.outputs.position, query.outputs.faceId);
}

}// end namespace ph
