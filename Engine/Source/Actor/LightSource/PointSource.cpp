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
#include "Math/constant.h"
#include "Core/Texture/TConstantTexture.h"

#include <iostream>
#include <memory>

namespace ph
{

PointSource::PointSource() :
	PointSource(Vector3R(1, 1, 1), 1000.0_r)
{}

PointSource::PointSource(const Vector3R& linearSrgbColor, const real numWatts) :
	LightSource(),
	m_color(), m_numWatts(numWatts)
{
	PH_ASSERT(numWatts > 0.0_r);

	m_color.setLinearSrgb(linearSrgbColor, EQuantity::EMR);
}

PointSource::PointSource(const SampledSpectralStrength& color, const real numWatts) :
	LightSource(),
	m_color(color), m_numWatts(numWatts)
{
	PH_ASSERT(numWatts > 0.0_r);
}

PointSource::~PointSource() = default;

std::unique_ptr<Emitter> PointSource::genEmitter(
	CookingContext& context, EmitterBuildingMaterial&& data) const
{
	PH_ASSERT_MSG(data.primitives.empty(), "primitive data not required");

	const real SPHERE_RADIUS = 0.005_r;

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
		auto smallSphere            = std::make_unique<PSphere>(metadata, SPHERE_RADIUS);
		auto transformedSmallSphere = std::make_unique<TransformedPrimitive>(smallSphere.get(), 
		                                                                     baseLW, 
		                                                                     baseWL);
		primitive = transformedSmallSphere.get();

		cookedUnit.intersectables.push_back(std::move(transformedSmallSphere));
		context.addBackend(std::move(smallSphere));
	}
	PH_ASSERT(primitive != nullptr);

	const auto unitWattColor   = m_color.div(m_color.sum());
	const auto totalWattColor  = unitWattColor.mul(m_numWatts);
	const real lightArea       = 4.0_r * PH_PI_REAL * SPHERE_RADIUS * SPHERE_RADIUS;
	const auto lightIrradiance = totalWattColor.div(lightArea);
	const auto lightRadiance   = lightIrradiance.div(2.0_r * PH_PI_REAL);

	SpectralStrength radiance;
	radiance.setSampled(lightRadiance, EQuantity::EMR);
	const auto& emittedRadiance = std::make_shared<TConstantTexture<SpectralStrength>>(radiance);

	auto emitter = std::make_unique<PrimitiveAreaEmitter>(primitive);
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
	rgbInput.addVector3r("linear-srgb");
	rgbInput.addReal("watts");

	if(packet.isPrototypeMatched(rgbInput))
	{
		const auto& linearSrgbColor = packet.getVector3r(
			"linear-srgb", Vector3R(0), DataTreatment::REQUIRED());
		const real watts = packet.getReal("watts", 1000.0_r, DataTreatment::REQUIRED());

		return std::make_unique<PointSource>(linearSrgbColor, watts);
	}
	else
	{
		std::cerr << "warning: at PointSource::loadPointSource(), "
		          << "invalid input format" << std::endl;
		return std::make_unique<PointSource>();
	}
}

}// end namespace ph