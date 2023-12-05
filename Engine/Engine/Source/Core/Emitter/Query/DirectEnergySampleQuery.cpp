#include "Core/Emitter/Query/DirectEnergySampleQuery.h"

#include <Common/assertion.h>

namespace ph
{

DirectEnergySampleInput::DirectEnergySampleInput() :
	targetPos(0)
{}

void DirectEnergySampleInput::set(const math::Vector3R& targetPos)
{
	this->targetPos = targetPos;
}

DirectEnergySampleOutput::DirectEnergySampleOutput() :
	emitPos     (0),
	radianceLe  (0),
	pdfW        (0),
	srcPrimitive(nullptr)
{
	PH_ASSERT(!(*this));
}

DirectEnergySampleOutput::operator bool () const
{
	return pdfW > 0;
}

DirectEnergySampleQuery::DirectEnergySampleQuery() :
	in(), out()
{}

}// end namespace ph
