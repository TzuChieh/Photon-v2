#include "Actor/Light/AAreaLight.h"
#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Core/Emitter/GroupedDiffuseSurfaceEmitter.h"
#include "Actor/Image/ConstantImage.h"
#include "Math/constant.h"
#include "World/Foundation/CookingContext.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "Core/Texture/constant_textures.h"
#include "Actor/Material/IdealSubstance.h"
#include "Math/Color/spectral_samples.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <vector>
#include <utility>

namespace ph
{

std::shared_ptr<Geometry> AAreaLight::getGeometry(const CookingContext& ctx) const
{
	std::shared_ptr<Geometry> area = getArea(ctx);
	PH_ASSERT(area);
	return area;
}

std::shared_ptr<Material> AAreaLight::getMaterial(const CookingContext& ctx) const
{
	// TODO: transmitter if not want to cast shadow

	auto material = TSdl<IdealSubstance>::makeResource();
	material->setSubstance(EIdealSubstance::Absorber);
	return material;
}

const Emitter* AAreaLight::buildEmitter(
	const CookingContext& ctx,
	TSpanView<const Primitive*> lightPrimitives) const
{
	if(lightPrimitives.empty())
	{
		PH_DEFAULT_LOG(Error,
			"failed building area light emitter: requires at least a light primitive");
		return nullptr;
	}

	real lightArea = 0.0_r;
	for(const Primitive* primitive : lightPrimitives)
	{
		lightArea += primitive->calcExtendedArea();
	}
	PH_ASSERT_GT(lightArea, 0.0_r);

	// Compute uniform surface radiance from watts
	PH_ASSERT_GT(m_color.abs().sum(), 0.0_r);
	const auto totalWattColor = math::Spectrum(m_color).putEnergy(m_numWatts);
	const auto lightRadiance  = totalWattColor / (lightArea * math::constant::pi<real>);

	auto emittedRadiance = std::make_shared<TConstantTexture<math::Spectrum>>(lightRadiance);
	if(!lightRadiance.isFinite())
	{
		PH_DEFAULT_LOG(Warning,
			"Area light has non-finite radiance {}. Setting it to unit D65 instead.",
			lightRadiance.toString());

		const auto defaultRadiance = math::Spectrum().setSpectral(
			math::resample_illuminant_D65<math::ColorValue>(), math::EColorUsage::EMR);
		emittedRadiance = std::make_shared<TConstantTexture<math::Spectrum>>(defaultRadiance);
	}

	const Emitter* lightEmitter = nullptr;
	if(lightPrimitives.size() == 1)
	{
		lightEmitter = ctx.getResources()->makeEmitter<DiffuseSurfaceEmitter>(
			lightPrimitives[0], emittedRadiance, getEmitterFeatureSet());
	}
	else
	{
		PH_ASSERT_GE(lightPrimitives.size(), 2);

		lightEmitter = ctx.getResources()->makeEmitter<GroupedDiffuseSurfaceEmitter>(
			lightPrimitives, emittedRadiance, getEmitterFeatureSet());
	}

	return lightEmitter;
}

}// end namespace ph
