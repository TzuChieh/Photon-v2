#include "Core/CookedActor.h"

namespace ph
{

CookedActor::CookedActor() :
	primitives(), primitiveMetadata(nullptr), emitter(nullptr)
{

}

CookedActor::CookedActor(CookedActor&& other) :
	primitives(std::move(other.primitives)), primitiveMetadata(std::move(other.primitiveMetadata)), emitter(std::move(other.emitter))
{

}

CookedActor& CookedActor::operator = (CookedActor&& rhs)
{
	primitives        = std::move(rhs.primitives);
	primitiveMetadata = std::move(rhs.primitiveMetadata);
	emitter           = std::move(rhs.emitter);

	return *this;
}

}// end namespace ph