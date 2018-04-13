#include "Actor/LightSource/AreaSource.h"
#include "Actor/CookingContext.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "Math/Transform/RigidTransform.h"
#include "Math/Transform/StaticRigidTransform.h"
#include "Core/Intersectable/PSphere.h"
#include "Core/Intersectable/TransformedPrimitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Common/assertion.h"
#include "Core/Emitter/PrimitiveAreaEmitter.h"
#include "Core/Emitter/MultiAreaEmitter.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LambertianDiffuse.h"
#include "Actor/Image/ConstantImage.h"
#include "FileIO/InputPacket.h"
#include "FileIO/InputPrototype.h"
#include "Actor/Image/LdrPictureImage.h"
#include "FileIO/PictureLoader.h"
#include "Math/constant.h"
#include "Core/Texture/TConstantTexture.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Actor/AModel.h"

#include <iostream>
#include <memory>

namespace ph
{

AreaSource::AreaSource() :
	AreaSource(Vector3R(1, 1, 1), 100.0_r)
{}

AreaSource::AreaSource(const Vector3R& linearSrgbColor, const real numWatts) :
	LightSource(),
	m_color(), m_numWatts(numWatts)
{
	PH_ASSERT(numWatts > 0.0_r);

	m_color.setLinearSrgb(linearSrgbColor, EQuantity::EMR);
}

AreaSource::AreaSource(const SampledSpectralStrength& color, const real numWatts) :
	LightSource(),
	m_color(color), m_numWatts(numWatts)
{
	PH_ASSERT(numWatts > 0.0_r);
}

AreaSource::~AreaSource() = default;

std::unique_ptr<Emitter> AreaSource::genEmitter(
	CookingContext& context, EmitterBuildingMaterial&& data) const
{
	if(data.primitives.empty())
	{
		std::cerr << "warning: at AreaSource::genEmitter(), "
		          << "requires at least a primitive, emitter ignored" << std::endl;
		return nullptr;
	}

	std::vector<const Primitive*> areas = std::move(data.primitives);

	real lightArea = 0.0_r;
	for(const auto area : areas)
	{
		lightArea += area->calcExtendedArea();
	}
	PH_ASSERT(lightArea > 0.0_r);

	const auto unitWattColor  = m_color.div(m_color.sum());
	const auto totalWattColor = unitWattColor.mul(m_numWatts);
	const auto lightRadiance  = totalWattColor.div(lightArea * PH_PI_REAL);

	SpectralStrength radiance;
	radiance.setSampled(lightRadiance, EQuantity::EMR);
	const auto& emittedRadiance = std::make_shared<TConstantTexture<SpectralStrength>>(radiance);

	std::unique_ptr<Emitter> emitter;
	{
		if(areas.size() > 1)
		{
			std::vector<PrimitiveAreaEmitter> areaEmitters;
			for(auto area : areas)
			{
				areaEmitters.push_back(PrimitiveAreaEmitter(area));
			}

			auto emitterData = std::make_unique<MultiAreaEmitter>(areaEmitters);
			emitterData->setEmittedRadiance(emittedRadiance);
			emitter = std::move(emitterData);
		}
		else
		{
			auto emitterData = std::make_unique<PrimitiveAreaEmitter>(areas[0]);
			emitterData->setEmittedRadiance(emittedRadiance);
			emitter = std::move(emitterData);
		}
	}
	PH_ASSERT(emitter != nullptr);

	return emitter;
}

std::shared_ptr<Geometry> AreaSource::genGeometry(CookingContext& context) const
{
	std::shared_ptr<Geometry> areas = genAreas(context);
	PH_ASSERT(areas != nullptr);
	return areas;
}

// command interface

AreaSource::AreaSource(const InputPacket& packet) : 
	LightSource(packet)
{
	InputPrototype rgbInput;
	rgbInput.addVector3r("linear-srgb");
	rgbInput.addReal("watts");

	if(packet.isPrototypeMatched(rgbInput))
	{
		m_color.setLinearSrgb(packet.getVector3r("linear-srgb"));
		m_numWatts = packet.getReal("watts");
	}
	else
	{
		std::cerr << "warning: at AreaSource ctor, "
		          << "invalid input format" << std::endl;
		m_color.setLinearSrgb(Vector3R(1, 1, 1));
		m_numWatts = 100.0_r;
	}
}

SdlTypeInfo AreaSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "area");
}

void AreaSource::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph