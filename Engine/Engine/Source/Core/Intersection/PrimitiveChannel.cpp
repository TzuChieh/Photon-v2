#include "Core/Intersection/PrimitiveChannel.h"
#include "Core/Intersection/UvwMapper/SphericalMapper.h"

#include <Common/assertion.h>

namespace ph
{

PrimitiveChannel::PrimitiveChannel() :
	PrimitiveChannel(std::make_shared<SphericalMapper>())
{}

PrimitiveChannel::PrimitiveChannel(const std::shared_ptr<UvwMapper>& mapper) :
	m_mapper(mapper)
{
	PH_ASSERT(mapper);
}

}// end namespace ph
