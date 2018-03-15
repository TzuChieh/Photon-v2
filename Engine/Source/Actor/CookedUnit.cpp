#include "Actor/CookedUnit.h"

namespace ph
{

CookedUnit::CookedUnit() :
	intersectables(), 
	primitiveMetadatas(), 
	emitters(),
	transforms()
{}

CookedUnit::CookedUnit(CookedUnit&& other) :
	intersectables    (std::move(other.intersectables)), 
	primitiveMetadatas(std::move(other.primitiveMetadatas)), 
	emitters          (std::move(other.emitters)),
	transforms        (std::move(other.transforms))
{}

CookedUnit& CookedUnit::operator = (CookedUnit&& rhs)
{
	intersectables     = std::move(rhs.intersectables);
	primitiveMetadatas = std::move(rhs.primitiveMetadatas);
	emitters           = std::move(rhs.emitters);
	transforms         = std::move(rhs.transforms);

	return *this;
}

}// end namespace ph