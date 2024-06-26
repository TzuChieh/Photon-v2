#include "Actor/LightSource/AreaSource.h"
#include "World/Foundation/CookingContext.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "Math/Transform/RigidTransform.h"
#include "Math/Transform/StaticRigidTransform.h"
#include "Core/Intersection/TransformedPrimitive.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Core/Emitter/GroupedDiffuseSurfaceEmitter.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LambertianDiffuse.h"
#include "Actor/Image/ConstantImage.h"
#include "Math/constant.h"
#include "Core/Texture/constant_textures.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Math/Color/color_spaces.h"

#include <Common/assertion.h>

#include <iostream>
#include <memory>

namespace ph
{

AreaSource::AreaSource() :
	AreaSource(math::Vector3R(1, 1, 1), 100.0_r)
{}

AreaSource::AreaSource(const math::Vector3R& linearSrgbColor, const real numWatts) :
	LightSource(),
	m_color(), m_numWatts(numWatts)
{
	PH_ASSERT_GT(numWatts, 0.0_r);

	m_color.setLinearSRGB(linearSrgbColor.toArray(), math::EColorUsage::EMR);
}

AreaSource::AreaSource(const math::Spectrum& color, const real numWatts) :
	LightSource(),
	m_color(color), m_numWatts(numWatts)
{
	PH_ASSERT_GT(numWatts, 0.0_r);
}

std::unique_ptr<Emitter> AreaSource::genEmitter(
	CookingContext& ctx, EmitterBuildingMaterial&& data) const
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
	PH_ASSERT_GT(lightArea, 0.0_r);

	PH_ASSERT_GT(m_color.abs().sum(), 0.0_r);
	const auto totalWattColor = math::Spectrum(m_color).putEnergy(m_numWatts);
	const auto lightRadiance  = totalWattColor / (lightArea * math::constant::pi<real>);

	const auto& emittedRadiance = std::make_shared<TConstantTexture<math::Spectrum>>(lightRadiance);

	std::unique_ptr<Emitter> emitter;
	/*{
		if(areas.size() > 1)
		{
			std::vector<DiffuseSurfaceEmitter> areaEmitters;
			for(auto area : areas)
			{
				areaEmitters.push_back(DiffuseSurfaceEmitter(area));
			}

			auto emitterData = std::make_unique<MultiDiffuseSurfaceEmitter>(areaEmitters);
			emitterData->setEmittedEnergy(emittedRadiance);
			emitter = std::move(emitterData);
		}
		else
		{
			auto emitterData = std::make_unique<DiffuseSurfaceEmitter>(areas[0]);
			emitterData->setEmittedEnergy(emittedRadiance);
			emitter = std::move(emitterData);
		}
	}*/
	PH_ASSERT(emitter != nullptr);

	return emitter;
}

std::shared_ptr<Geometry> AreaSource::genGeometry(CookingContext& ctx) const
{
	std::shared_ptr<Geometry> areas = genAreas(ctx);
	PH_ASSERT(areas != nullptr);
	return areas;
}

}// end namespace ph
