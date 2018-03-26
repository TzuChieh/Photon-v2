#include "Actor/LightSource/PointSource.h"
#include "Actor/CookingContext.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "Math/Transform/RigidTransform.h"
#include "Math/Transform/StaticRigidTransform.h"
#include "Core/Intersectable/PSphere.h"
#include "Core/Intersectable/TransformedPrimitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Common/assertion.h"
#include "Core/Emitter/PrimitiveAreaEmitter.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LambertianDiffuse.h"
#include "Actor/Image/ConstantImage.h"
#include "FileIO/InputPacket.h"
#include "FileIO/InputPrototype.h"
#include "Actor/Image/LdrPictureImage.h"
#include "FileIO/PictureLoader.h"

#include <iostream>
#include <memory>

namespace ph
{

PointSource::PointSource() :
	PointSource(std::make_shared<ConstantImage>(1000.0_r, 
	                                            ConstantImage::EType::EMR_LINEAR_SRGB))
{}

PointSource::PointSource(const Vector3R& linearSrgbRadiance) : 
	PointSource(std::make_shared<ConstantImage>(linearSrgbRadiance, 
	                                            ConstantImage::EType::EMR_LINEAR_SRGB))
{}

PointSource::PointSource(const std::shared_ptr<Image> radiance) : 
	LightSource(),
	m_emittedRadiance(radiance)
{
	PH_ASSERT(radiance != nullptr);
}

PointSource::~PointSource() = default;

std::unique_ptr<Emitter> PointSource::genEmitter(
	CookingContext& context, EmitterBuildingMaterial&& data) const
{
	PH_ASSERT_MSG(data.primitives.empty(), "primitive data not required");

	CookedUnit cookedUnit;

	RigidTransform* baseLW = nullptr;
	RigidTransform* baseWL = nullptr;
	if(data.baseLocalToWorld != nullptr && data.baseWorldToLocal != nullptr)
	{
		baseLW = data.baseLocalToWorld.get();
		baseWL = data.baseWorldToLocal.get();
		cookedUnit.transforms.push_back(std::move(data.baseLocalToWorld));
		cookedUnit.transforms.push_back(std::move(data.baseWorldToLocal));
	}
	else
	{
		std::cerr << "warning: at PointSource::genEmitter(), "
		          << "incomplete transform information, use identity transform instead" << std::endl;
		auto identity = std::make_unique<StaticRigidTransform>(StaticRigidTransform::makeIdentity());
		baseLW = identity.get();
		baseWL = identity.get();
		cookedUnit.transforms.push_back(std::move(identity));
	}
	PH_ASSERT(baseLW != nullptr && baseWL != nullptr);

	PrimitiveMetadata* metadata = nullptr;
	{
		auto primitiveMetadata = std::make_unique<PrimitiveMetadata>();
		metadata = primitiveMetadata.get();
		cookedUnit.primitiveMetadatas.push_back(std::move(primitiveMetadata));
	}
	PH_ASSERT(metadata != nullptr);

	Primitive* primitive = nullptr;
	{
		auto smallSphere            = std::make_unique<PSphere>(metadata, 0.005_r);
		auto transformedSmallSphere = std::make_unique<TransformedPrimitive>(smallSphere.get(), 
		                                                                     baseLW, 
		                                                                     baseWL);
		primitive = transformedSmallSphere.get();

		cookedUnit.intersectables.push_back(std::move(transformedSmallSphere));
		context.addBackend(std::move(smallSphere));
	}
	PH_ASSERT(primitive != nullptr);
	
	auto emitter         = std::make_unique<PrimitiveAreaEmitter>(primitive);
	auto emittedRadiance = m_emittedRadiance->genTextureSpectral(context);
	emitter->setEmittedRadiance(emittedRadiance);

	metadata->surfaceBehavior.setSurfaceOptics(std::make_unique<LambertianDiffuse>());
	metadata->surfaceBehavior.setEmitter(emitter.get());

	context.addCookedUnit(std::move(cookedUnit));
	return emitter;
}

// command interface

SdlTypeInfo PointSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "point");
}

void PointSource::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader(loadPointSource));
}

std::unique_ptr<PointSource> PointSource::loadPointSource(const InputPacket& packet)
{
	InputPrototype rgbInput;
	rgbInput.addVector3r("emitted-radiance");

	InputPrototype pictureFilenameInput;
	pictureFilenameInput.addString("emitted-radiance");

	if(packet.isPrototypeMatched(rgbInput))
	{
		const auto& emittedRadiance = packet.getVector3r(
			"emitted-radiance", Vector3R(0), DataTreatment::REQUIRED());
		return std::make_unique<PointSource>(emittedRadiance);

	}
	else if(packet.isPrototypeMatched(pictureFilenameInput))
	{
		const auto& imagePath = packet.getStringAsPath(
			"emitted-radiance", Path(), DataTreatment::REQUIRED());
		const auto& image = std::make_shared<LdrPictureImage>(PictureLoader::loadLdr(imagePath));
		return std::make_unique<PointSource>(image);
	}
	else
	{
		const auto& image = packet.get<Image>(
			"emitted-radiance", DataTreatment::REQUIRED());
		return std::make_unique<PointSource>(image);
	}

	std::cerr << "warning: at PointSource::ciLoad(), invalid input format" << std::endl;
	return std::make_unique<PointSource>();
}

}// end namespace ph