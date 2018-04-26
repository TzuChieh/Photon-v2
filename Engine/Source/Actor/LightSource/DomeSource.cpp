#include "Actor/LightSource/DomeSource.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Material/IdealSubstance.h"
#include "Actor/CookingContext.h"
#include "World/VisualWorldInfo.h"
#include "Core/Emitter/PrimitiveAreaEmitter.h"
#include "Core/Emitter/MultiAreaEmitter.h"
#include "FileIO/PictureLoader.h"
#include "Actor/Image/HdrPictureImage.h"
#include "Common/assertion.h"
#include "FileIO/InputPacket.h"

namespace ph
{

const Logger DomeSource::logger(LogSender("Dome Source"));

DomeSource::DomeSource() : 
	DomeSource(Path())
{}

DomeSource::DomeSource(const Path& sphericalEnvMap) : 
	LightSource(),
	m_sphericalEnvMap(sphericalEnvMap)
{}

DomeSource::~DomeSource() = default;

// TODO: specify uvw mapper explicitly

std::unique_ptr<Emitter> DomeSource::genEmitter(
	CookingContext& context, EmitterBuildingMaterial&& data) const
{
	if(data.primitives.empty())
	{
		logger.log(ELogLevel::WARNING_MED, 
			"no primitive provided; requires at least a primitive to build emitter");
		return nullptr;
	}

	auto frame = PictureLoader::loadHdr(m_sphericalEnvMap);
	frame.flipHorizontally();// since we are viewing it from inside a sphere

	auto image = std::make_shared<HdrPictureImage>(std::move(frame));
	image->setSampleMode(EImgSampleMode::BILINEAR);
	image->setWrapMode(EImgWrapMode::REPEAT);
	auto emittedRadiance = image->genTextureSpectral(context);

	std::vector<PrimitiveAreaEmitter> primitiveEmitters;
	for(const auto& primitive : data.primitives)
	{
		PrimitiveAreaEmitter emitter(primitive);
		emitter.setEmittedRadiance(emittedRadiance);
		primitiveEmitters.push_back(emitter);
	}

	std::unique_ptr<SurfaceEmitter> emitter;
	if(primitiveEmitters.size() == 1)
	{
		emitter = std::make_unique<PrimitiveAreaEmitter>(primitiveEmitters[0]);
	}
	else
	{
		PH_ASSERT(!primitiveEmitters.empty());

		auto multiEmitter = std::make_unique<MultiAreaEmitter>(std::move(primitiveEmitters));
		multiEmitter->setEmittedRadiance(emittedRadiance);
		emitter = std::move(multiEmitter);
	}

	PH_ASSERT(emitter != nullptr);

	// We are inside a large sphere, so we need to make back face emitable.
	//
	emitter->setBackFaceEmit();

	return emitter;
}

std::shared_ptr<Geometry> DomeSource::genGeometry(CookingContext& context) const
{
	real rootActorBoundRadius = 1000.0_r;
	if(context.getVisualWorldInfo())
	{
		const AABB3D&  bound   = context.getVisualWorldInfo()->getRootActorsBound();
		const Vector3R extends = bound.calcExtents();

		// Enlarge the root actor bound radius by this factor;
		// notice that if this radius is too small the rendered dome may 
		// exhibit distorsion even though the environment map is undistorted.
		//
		const real magnifier = 64.0_r;

		rootActorBoundRadius = extends.max() * magnifier;
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"No visual world information available, cannot access root actor bounds."
			"Using " + std::to_string(rootActorBoundRadius) + " as dome radius.");
	}

	return std::make_shared<GSphere>(rootActorBoundRadius);
}

std::shared_ptr<Material> DomeSource::genMaterial(CookingContext& context) const
{
	auto material = std::make_shared<IdealSubstance>();
	
	// A dome should not have any visible inter-reflection ideally.
	//
	material->asAbsorber();

	return material;
}

// command interface

DomeSource::DomeSource(const InputPacket& packet) : 
	LightSource(packet),
	m_sphericalEnvMap()
{
	// TODO: load env map file path
}

SdlTypeInfo DomeSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "dome");
}

void DomeSource::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<DomeSource>(packet);
	}));
}

}// end namespace ph