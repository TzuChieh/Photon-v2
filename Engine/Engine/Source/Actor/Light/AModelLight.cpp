#include "Actor/Light/AModelLight.h"
#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Core/Emitter/GroupedDiffuseSurfaceEmitter.h"
#include "World/Foundation/CookingContext.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "Core/Texture/constant_textures.h"
#include "Math/Color/spectral_samples.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <utility>

namespace ph
{

std::shared_ptr<Geometry> AModelLight::getGeometry(const CookingContext& ctx) const
{
	return m_geometry;
}

std::shared_ptr<Material> AModelLight::getMaterial(const CookingContext& ctx) const
{
	return m_material;
}

const Emitter* AModelLight::buildEmitter(
	const CookingContext& ctx,
	TSpanView<const Primitive*> lightPrimitives) const
{
	if(lightPrimitives.empty())
	{
		PH_DEFAULT_LOG(Error,
			"Failed building model light emitter: requires at least a light primitive.");
		return nullptr;
	}

	// Do not assume a specific energy unit (most likely being radiance though)
	std::shared_ptr<TTexture<math::Spectrum>> emittedEnergy;
	if(m_emittedEnergy)
	{
		emittedEnergy = m_emittedEnergy->genColorTexture(ctx);
	}
	else
	{
		PH_DEFAULT_LOG(Warning,
			"Model light does not specify emitted energy. Default to unit D65.");

		const auto defaultEnergy = math::Spectrum().setSpectral(
			math::resample_illuminant_D65<math::ColorValue>(), math::EColorUsage::EMR);
		emittedEnergy = std::make_shared<TConstantTexture<math::Spectrum>>(defaultEnergy);
	}

	SurfaceEmitter* lightEmitter = nullptr;
	if(lightPrimitives.size() == 1)
	{
		lightEmitter = ctx.getResources()->makeEmitter<DiffuseSurfaceEmitter>(
			lightPrimitives[0], emittedEnergy, getEmitterFeatureSet());
	}
	else
	{
		PH_ASSERT_GE(lightPrimitives.size(), 2);

		lightEmitter = ctx.getResources()->makeEmitter<GroupedDiffuseSurfaceEmitter>(
			lightPrimitives, emittedEnergy, getEmitterFeatureSet());
	}

	if(m_isBackFaceEmit)
	{
		lightEmitter->setBackFaceEmit();
	}
	else
	{
		lightEmitter->setFrontFaceEmit();
	}

	return lightEmitter;
}

void AModelLight::setGeometry(const std::shared_ptr<Geometry>& geometry)
{
	PH_ASSERT(geometry);
	m_geometry = geometry;
}

void AModelLight::setMaterial(const std::shared_ptr<Material>& material)
{
	// Material can be null
	m_material = material;
}

void AModelLight::setBackFaceEmit(bool isBackFaceEmit)
{
	m_isBackFaceEmit = isBackFaceEmit;
}

}// end namespace ph
