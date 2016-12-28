#include "Core/CoreActor.h"

namespace ph
{

CoreActor::CoreActor() :
	primitives(), primitiveMetadata(nullptr), emitter(nullptr)
{

}

CoreActor::CoreActor(CoreActor&& other) : 
	primitives(std::move(other.primitives)), primitiveMetadata(std::move(other.primitiveMetadata)), emitter(std::move(other.emitter))
{

}

CoreActor& CoreActor::operator = (CoreActor&& rhs)
{
	primitives        = std::move(rhs.primitives);
	primitiveMetadata = std::move(rhs.primitiveMetadata);
	emitter           = std::move(rhs.emitter);

	return *this;
}

}// end namespace ph