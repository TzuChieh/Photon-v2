#include "Actor/APhantomModel.h"
#include "Math/math.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Actor/CookedUnit.h"
#include "FileIO/SDL/InputPacket.h"
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

CookedUnit APhantomModel::cook(CookingContext& context) const
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

// command interface

APhantomModel::APhantomModel(const InputPacket& packet) :
	AModel(packet)
{
	m_phantomName = packet.getString("name", "", DataTreatment::REQUIRED());
}

SdlTypeInfo APhantomModel::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ACTOR, "phantom-model");
}

void APhantomModel::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<APhantomModel>(packet);
	}));

	SdlExecutor translateSE;
	translateSE.setName("translate");
	translateSE.setFunc<APhantomModel>(ciTranslate);
	cmdRegister.addExecutor(translateSE);

	SdlExecutor rotateSE;
	rotateSE.setName("rotate");
	rotateSE.setFunc<APhantomModel>(ciRotate);
	cmdRegister.addExecutor(rotateSE);

	SdlExecutor scaleSE;
	scaleSE.setName("scale");
	scaleSE.setFunc<APhantomModel>(ciScale);
	cmdRegister.addExecutor(scaleSE);
}

}// end namespace ph
