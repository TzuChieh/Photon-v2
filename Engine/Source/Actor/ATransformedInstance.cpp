#include "Actor/ATransformedInstance.h"
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
#include "Common/Logger.h"
#include "Common/assertion.h"

#include <algorithm>
#include <iostream>
#include <memory>

namespace ph
{

namespace
{
	const Logger logger(LogSender("Transformed Instance"));
}

ATransformedInstance::ATransformedInstance() :
	PhysicalActor()
{}

ATransformedInstance::ATransformedInstance(const ATransformedInstance& other) :
	PhysicalActor(other),
	m_phantomName(other.m_phantomName)
{}

ATransformedInstance& ATransformedInstance::operator = (ATransformedInstance rhs)
{
	swap(*this, rhs);

	return *this;
}

CookedUnit ATransformedInstance::cook(CookingContext& context) const
{
	CookedUnit cooked;

	const CookedUnit* phantom = context.getPhantom(m_phantomName);
	if(!phantom)
	{
		logger.log(ELogLevel::WARNING_MED,
			"phantom <" + m_phantomName + "> not found");

		return cooked;
	}

	if(phantom->intersectables().size() != 1)
	{
		logger.log(ELogLevel::WARNING_MED,
			"phantom <" + m_phantomName + "> contains unsupported data");

		return cooked;
	}

	auto baseLW = std::make_unique<StaticAffineTransform>(StaticAffineTransform::makeForward(m_localToWorld));
	auto baseWL = std::make_unique<StaticAffineTransform>(StaticAffineTransform::makeInverse(m_localToWorld));

	auto transformedTarget = std::make_unique<TransformedIntersectable>(
		phantom->intersectables().front().get(),
		baseLW.get(),
		baseWL.get());

	cooked.addIntersectable(std::move(transformedTarget));
	cooked.addTransform(std::move(baseLW));
	cooked.addTransform(std::move(baseWL));

	return cooked;
}

void swap(ATransformedInstance& first, ATransformedInstance& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(static_cast<PhysicalActor&>(first), static_cast<PhysicalActor&>(second));
	swap(first.m_phantomName,                second.m_phantomName);
}

// command interface

ATransformedInstance::ATransformedInstance(const InputPacket& packet) :
	PhysicalActor(packet)
{
	m_phantomName = packet.getString("name", "", DataTreatment::REQUIRED());
}

SdlTypeInfo ATransformedInstance::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ACTOR, "transformed-instance");
}

void ATransformedInstance::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<ATransformedInstance>(packet);
	}));

	SdlExecutor translateSE;
	translateSE.setName("translate");
	translateSE.setFunc<ATransformedInstance>(ciTranslate);
	cmdRegister.addExecutor(translateSE);

	SdlExecutor rotateSE;
	rotateSE.setName("rotate");
	rotateSE.setFunc<ATransformedInstance>(ciRotate);
	cmdRegister.addExecutor(rotateSE);

	SdlExecutor scaleSE;
	scaleSE.setName("scale");
	scaleSE.setFunc<ATransformedInstance>(ciScale);
	cmdRegister.addExecutor(scaleSE);
}

}// end namespace ph