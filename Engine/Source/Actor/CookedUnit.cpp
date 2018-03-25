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

CookedUnit& CookedUnit::add(CookedUnit&& cookedUnit)
{
	intersectables.insert(
		intersectables.end(),
		std::make_move_iterator(cookedUnit.intersectables.begin()),
		std::make_move_iterator(cookedUnit.intersectables.end()));

	primitiveMetadatas.insert(
		primitiveMetadatas.end(),
		std::make_move_iterator(cookedUnit.primitiveMetadatas.begin()),
		std::make_move_iterator(cookedUnit.primitiveMetadatas.end()));

	emitters.insert(
		emitters.end(),
		std::make_move_iterator(cookedUnit.emitters.begin()),
		std::make_move_iterator(cookedUnit.emitters.end()));

	transforms.insert(
		transforms.end(),
		std::make_move_iterator(cookedUnit.transforms.begin()),
		std::make_move_iterator(cookedUnit.transforms.end()));

	return *this;
}

CookedUnit& CookedUnit::operator = (CookedUnit&& rhs)
{
	intersectables     = std::move(rhs.intersectables);
	primitiveMetadatas = std::move(rhs.primitiveMetadatas);
	emitters           = std::move(rhs.emitters);
	transforms         = std::move(rhs.transforms);

	return *this;
}

}// end namespace ph