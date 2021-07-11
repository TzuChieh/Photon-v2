#include "Actor/APhantomModel.h"
#include "Math/math.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Actor/CookedUnit.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Core/Intersectable/TransformedIntersectable.h"
#include "Actor/MotionSource/MotionSource.h"
#include "Core/Quantity/Time.h"
#include "Actor/ModelBuilder.h"
#include "Actor/CookingContext.h"
#include "Core/Intersectable/Bvh/ClassicBvhIntersector.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

namespace ph
{

APhantomModel::APhantomModel() :
	AModel()
{}

APhantomModel::APhantomModel(
	const std::shared_ptr<Geometry>& geometry,
	const std::shared_ptr<Material>& material) : 
	AModel(geometry, material)
{}

APhantomModel::APhantomModel(const APhantomModel& other) :
	AModel(other),
	m_phantomName(other.m_phantomName)
{}

APhantomModel& APhantomModel::operator = (APhantomModel rhs)
{
	swap(*this, rhs);

	return *this;
}

CookedUnit APhantomModel::cook(ActorCookingContext& ctx)
{
	CookedUnit cooked = AModel::cook(context);

	std::vector<const Intersectable*> intersectables;
	for(auto& intersectable : cooked.intersectables())
	{
		intersectables.push_back(intersectable.get());
		cooked.addBackend(std::move(intersectable));
	}
	cooked.intersectables().clear();

	auto bvh = std::make_unique<ClassicBvhIntersector>();
	bvh->rebuildWithIntersectables(std::move(intersectables));
	cooked.addIntersectable(std::move(bvh));

	context.addPhantom(m_phantomName, std::move(cooked));

	return CookedUnit();
}

void swap(APhantomModel& first, APhantomModel& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(static_cast<AModel&>(first), static_cast<AModel&>(second));
	swap(first.m_phantomName,         second.m_phantomName);
}

}// end namespace ph
