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

#include <algorithm>

namespace ph
{
namespace
{
	const Logger logger(LogSender("Actor Dome"));
}

ADome::ADome() : 
	ADome(Path())
{}

ADome::ADome(const Path& sphericalEnvMap) :
	PhysicalActor(),
	m_sphericalEnvMap(sphericalEnvMap)
{}

ADome::ADome(const ADome& other) : 
	PhysicalActor(other),
	m_sphericalEnvMap(other.m_sphericalEnvMap)
{}

CookedUnit ADome::cook(CookingContext& context) const
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
	
	math::Vector2S resolution;
	auto radianceTexture = loadRadianceTexture(m_sphericalEnvMap, context, &resolution);

	auto domeEmitter = std::make_unique<LatLongEnvEmitter>(
		domePrimitive.get(),
		radianceTexture,
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

ADome& ADome::operator = (ADome rhs)
{
	swap(*this, rhs);

	return *this;
}

void swap(ADome& first, ADome& second)
{
	// enable ADL
	using std::swap;

	swap(static_cast<PhysicalActor&>(first), static_cast<PhysicalActor&>(second));
	swap(first.m_sphericalEnvMap,            second.m_sphericalEnvMap);
}

std::shared_ptr<TTexture<Spectrum>> ADome::loadRadianceTexture(
	const Path&           filePath,
	CookingContext&       context,
	math::Vector2S* const out_resolution)
{
	auto frame = PictureLoader::loadHdr(filePath);

	// Since we are viewing it from inside a sphere
	frame.flipHorizontally();

	if(out_resolution)
	{
		*out_resolution = math::Vector2S(frame.getSizePx());
	}

	auto image = std::make_shared<HdrPictureImage>(std::move(frame));
	image->setSampleMode(EImgSampleMode::BILINEAR);
	image->setWrapMode(EImgWrapMode::REPEAT);

	return image->genTextureSpectral(context);
}

// command interface

ADome::ADome(const InputPacket& packet) : 
	PhysicalActor(packet),
	m_sphericalEnvMap()
{
	m_sphericalEnvMap = packet.getStringAsPath("env-map", 
		Path(), DataTreatment::REQUIRED());
}

SdlTypeInfo ADome::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ACTOR, "dome");
}

void ADome::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<ADome>(packet);
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
