#include "Actor/ADome.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Material/IdealSubstance.h"
#include "Actor/CookingContext.h"
#include "World/VisualWorldInfo.h"
#include "DataIO/SDL/InputPacket.h"
#include "Math/Transform/StaticRigidTransform.h"
#include "Core/Intersectable/PLatLongEnvSphere.h"
#include "Core/Emitter/LatLongEnvEmitter.h"
#include "DataIO/PictureLoader.h"
#include "Actor/Image/HdrPictureImage.h"
#include "Actor/Dome/AImageDome.h"

#include <algorithm>

namespace ph
{
namespace
{
	const Logger logger(LogSender("Actor Dome"));
}

ADome::ADome() : 
	PhysicalActor()
{}

ADome::ADome(const ADome& other) : 
	PhysicalActor(other)
{}

CookedUnit ADome::cook(CookingContext& context)
{
	// Ensure reasonable transformation for the dome
	math::TDecomposedTransform<hiReal> sanifiedLocalToWorld = m_localToWorld;
	if(sanifiedLocalToWorld.hasScaleEffect())
	{
		logger.log(
			"Scale detected and is ignored; scaling on dome light should "
			"be avoided as it does not have any effect. If resizing the dome is "
			"desired, it should be done by changing its radius.");

		sanifiedLocalToWorld.setScale(1);
	}

	auto localToWorld = std::make_unique<math::StaticRigidTransform>(math::StaticRigidTransform::makeForward(sanifiedLocalToWorld));
	auto worldToLocal = std::make_unique<math::StaticRigidTransform>(math::StaticRigidTransform::makeInverse(sanifiedLocalToWorld));

	// Get the sphere radius that can encompass all actors
	real domeRadius = 1000.0_r;
	if(context.getVisualWorldInfo())
	{
		const auto worldBound = context.getVisualWorldInfo()->getLeafActorsBound();
		for(auto vertex : worldBound.getBoundVertices())
		{
			constexpr real ENLARGEMENT = 1.01_r;

			const auto vertexToCenter = (vertex - math::Vector3R(sanifiedLocalToWorld.getPosition()));
			const real ri             = vertexToCenter.length() * ENLARGEMENT;

			domeRadius = std::max(ri, domeRadius);
		}
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"No visual world information available, cannot access actor bounds."
			"Using " + std::to_string(domeRadius) + " as dome radius.");
	}

	auto metadata = std::make_unique<PrimitiveMetadata>();

	// A dome should not have any visible inter-reflections, ideally
	auto material = std::make_shared<IdealSubstance>();
	material->asAbsorber();
	material->genBehaviors(context, *metadata);

	auto domePrimitive = std::make_unique<PLatLongEnvSphere>(
		metadata.get(),
		domeRadius,
		localToWorld.get(),
		worldToLocal.get());
	
	auto radianceFunction = loadRadianceFunction(context);
	auto resolution       = getResolution();

	auto domeEmitter = std::make_unique<LatLongEnvEmitter>(
		domePrimitive.get(),
		radianceFunction,
		resolution);
	metadata->getSurface().setEmitter(domeEmitter.get());
	
	// Store cooked data

	CookedUnit cookedActor;
	cookedActor.setPrimitiveMetadata(std::move(metadata));
	cookedActor.addTransform(std::move(localToWorld));
	cookedActor.addTransform(std::move(worldToLocal));
	cookedActor.setEmitter(std::move(domeEmitter));

	context.setBackgroundPrimitive(std::move(domePrimitive));

	return cookedActor;
}

ADome& ADome::operator = (const ADome& rhs)
{
	PhysicalActor::operator = (rhs);

	return *this;
}

void swap(ADome& first, ADome& second)
{
	// Enable ADL
	using std::swap;

	swap(static_cast<PhysicalActor&>(first), static_cast<PhysicalActor&>(second));
}

// command interface

ADome::ADome(const InputPacket& packet) : 
	PhysicalActor(packet)
{}

SdlTypeInfo ADome::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ACTOR, "dome");
}

void ADome::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader(
		[](const InputPacket& packet) -> std::unique_ptr<ISdlResource>
		{
			const auto type = packet.getString("type", 
				"", DataTreatment::REQUIRED());

			if(type == "image")
			{
				return std::make_unique<AImageDome>(packet);
			}
			else
			{
				logger.log(ELogLevel::WARNING_MED,
					"unsupported dome type <" + type + ">");

				return nullptr;
			}
		}));

	SdlExecutor translateSE;
	translateSE.setName("translate");
	translateSE.setFunc<ADome>(ciTranslate);
	cmdRegister.addExecutor(translateSE);

	SdlExecutor rotateSE;
	rotateSE.setName("rotate");
	rotateSE.setFunc<ADome>(ciRotate);
	cmdRegister.addExecutor(rotateSE);

	SdlExecutor scaleSE;
	scaleSE.setName("scale");
	scaleSE.setFunc<ADome>(ciScale);
	cmdRegister.addExecutor(scaleSE);
}

}// end namespace ph
