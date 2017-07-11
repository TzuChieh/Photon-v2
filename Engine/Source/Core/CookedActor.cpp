#include "Core/CookedActor.h"

namespace ph
{

CookedActor::CookedActor() :
	intersectables(), 
	primitiveMetadata(nullptr), 
	emitter(nullptr)
{

}

CookedActor::CookedActor(CookedActor&& other) :
	intersectables(std::move(other.intersectables)), 
	primitiveMetadata(std::move(other.primitiveMetadata)), 
	emitter(std::move(other.emitter))
{

}

CookedActor& CookedActor::operator = (CookedActor&& rhs)
{
	intersectables    = std::move(rhs.intersectables);
	primitiveMetadata = std::move(rhs.primitiveMetadata);
	emitter           = std::move(rhs.emitter);

	return *this;
}

}// end namespace ph