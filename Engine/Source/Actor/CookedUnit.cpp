#include "Actor/CookedUnit.h"

namespace ph
{

CookedUnit::CookedUnit() :
	intersectables(), 
	primitiveMetadata(nullptr), 
	emitter(nullptr),
	transforms()
{

}

CookedUnit::CookedUnit(CookedUnit&& other) :
	intersectables   (std::move(other.intersectables)), 
	primitiveMetadata(std::move(other.primitiveMetadata)), 
	emitter          (std::move(other.emitter)),
	transforms       (std::move(other.transforms))
{

}

CookedUnit& CookedUnit::operator = (CookedUnit&& rhs)
{
	intersectables    = std::move(rhs.intersectables);
	primitiveMetadata = std::move(rhs.primitiveMetadata);
	emitter           = std::move(rhs.emitter);
	transforms        = std::move(rhs.transforms);

	return *this;
}

}// end namespace ph