#include "Engine/Actor/Light/AGeometricLight.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/Math/math.h"
#include "Engine/Actor/Material/MatteOpaque.h"
#include "Engine/SDL/TSdl.h"
#include "Engine/World/Foundation/TransientVisualElement.h"
#include "Engine/Core/Intersection/PrimitiveBuilder.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/Transform/StaticRigidTransform.h"
#include "Engine/World/Foundation/PreCookReport.h"
#include "Engine/World/Foundation/CookedGeometry.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <Common/logging.h>

#include <algorithm>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(AGeometricLight, Actor);

std::shared_ptr<Material> AGeometricLight::getMaterial(const CookingContext& ctx) const
{
	return TSdl<MatteOpaque>::makeResource();
}

void AGeometricLight::setShouldFlipNg(const bool shouldFlipNg)
{
	m_shouldFlipNg = shouldFlipNg;
}

bool AGeometricLight::shouldFlipNg() const
{
	return m_shouldFlipNg;
}

PreCookReport AGeometricLight::preCook(const CookingContext& ctx) const
{
	PreCookReport report = PhysicalActor::preCook(ctx);

	// TODO: test "isRigid()" may be more appropriate
	if(m_localToWorld.getDecomposed().hasScaleEffect() || m_localToWorld.getDecomposed().isIdentity())
	{
		// Scaled transforms are fully baked during geometry sanification; identity needs no wrapper
		report.setBaseTransforms(nullptr, nullptr);
	}
	else
	{
		// Rigid transforms can (and should) be pre-cooked
		auto* localToWorld = ctx.getResources().makeTransform<StaticRigidTransform>(
			m_localToWorld.getForwardStaticRigid());
		auto* worldToLocal = ctx.getResources().makeTransform<StaticRigidTransform>(
			m_localToWorld.getInverseStaticRigid());

		report.setBaseTransforms(localToWorld, worldToLocal);
	}

	return report;
}

TransientVisualElement AGeometricLight::cook(const CookingContext& ctx, const PreCookReport& report) const
{
	std::shared_ptr<Geometry> geometry = getGeometry(ctx);
	std::shared_ptr<Material> material = getMaterial(ctx);

	if(!geometry)
	{
		throw ActorCookException(
			"cannot build geometric light, please make sure the actor is geometric or supply a "
			"valid geometry resource");
	}

	if(!material)
	{
		PH_LOG(AGeometricLight, Note,
			"material is not specified, using default material");
		material = TSdl<MatteOpaque>::makeResource();
	}

	const CookedGeometry* cookedGeometry = getSanifiedGeometry(geometry, m_localToWorld, ctx);
	if(cookedGeometry->primitives.empty())
	{
		return TransientVisualElement();
	}

	// Our default policy is Ng facing is unaffectd by winding change, 
	// so if `isWindingFlipped` is true then that implies a flip, if `m_shouldFlipNg`
	// is specified additionally then they can cancel out
	const bool shouldFlipNg = m_shouldFlipNg != cookedGeometry->isWindingFlipped;

	PrimitiveMetadata* metadata = ctx.getResources().makeMetadata();
	metadata->setGeometryInfo(&cookedGeometry->geometryInfo);

	const CookedMaterial* cookedMaterial = ctx.getCooked(material);
	if(!cookedMaterial)
	{
		const auto materialKey = ctx.getKey(*material);
		CookedMaterial* const newCookedMaterial = ctx.getResources().makeMaterial(materialKey);
		material->cook(ctx, *newCookedMaterial);
		cookedMaterial = newCookedMaterial;
	}

	if(cookedMaterial->interfaceMask)
	{
		throw ActorCookException(
			"geometric light does not support masking");
	}
	
	metadata->surface().setOptics(cookedMaterial->surfaceOptics);

	if(isVolumetricEmissionSupported() && material->getOverlapPriority() > 0)
	{
		// Assuming the geometry has a closed shape, so its interior and exterior are well defined.
		// It is user's responsibility to not set the interior and exterior for open shapes.
		const VolumeOptics* interiorOptics = nullptr;
		const VolumeOptics* exteriorOptics = nullptr;
		cookedMaterial->findFirstCompatibleOptics(&interiorOptics, &exteriorOptics);

		metadata->interior().setOptics(interiorOptics);
		metadata->exterior().setOptics(exteriorOptics);
		metadata->setInteriorPriority(material->getOverlapPriority());
	}

	if(m_localToWorld.getDecomposed().isIdentity())
	{
		// Just to make sure we are not pre-cooking identity transforms
		PH_ASSERT(!report.getBaseLocalToWorld());
		PH_ASSERT(!report.getBaseWorldToLocal());
	}
	else if(!m_localToWorld.getDecomposed().hasScaleEffect())
	{
		// Can (and should) be pre-cooked
		PH_ASSERT(report.getBaseLocalToWorld());
		PH_ASSERT(report.getBaseWorldToLocal());
	}

	// Scaled transforms are fully baked. Otherwise, this must be rigid transform, see `preCook()`
	const auto* localToWorld = static_cast<const StaticRigidTransform*>(report.getBaseLocalToWorld());
	const auto* worldToLocal = static_cast<const StaticRigidTransform*>(report.getBaseWorldToLocal());

	std::vector<const Primitive*> lightPrimitives;
	lightPrimitives.reserve(cookedGeometry->primitives.size());
	for(const Primitive* primitive : cookedGeometry->primitives)
	{
		auto primitiveBuilder = PrimitiveBuilder::referencing(primitive)
			.injectMetadata(metadata);

		const Primitive* lightPrimitive;
		if(localToWorld)
		{
			PH_ASSERT(worldToLocal);
			if(shouldFlipNg)
			{
				lightPrimitive = ctx.getResources().copyIntersectable(
					primitiveBuilder.rigidTransform<true>(localToWorld, worldToLocal).build());
			}
			else
			{
				lightPrimitive = ctx.getResources().copyIntersectable(
					primitiveBuilder.rigidTransform(localToWorld, worldToLocal).build());
			}
		}
		else
		{
			if(shouldFlipNg)
			{
				lightPrimitive = ctx.getResources().copyIntersectable(
					primitiveBuilder.flipGeometryNormal().build());
			}
			else
			{
				lightPrimitive = ctx.getResources().copyIntersectable(
					primitiveBuilder.build());
			}
		}

		lightPrimitives.push_back(lightPrimitive);
	}

	TransientVisualElement cookedLight;
	for(const Primitive* primitive : lightPrimitives)
	{
		cookedLight.add(primitive);
	}

	const SurfaceEmitter* surfaceEmitter = buildSurfaceEmitter(ctx, lightPrimitives);
	if(!surfaceEmitter)
	{
		PH_LOG(AGeometricLight, Error,
			"no emitter generated");
		return cookedLight;
	}

	cookedLight.surfaceEmitters.push_back(surfaceEmitter);
	metadata->surface().setEmitter(surfaceEmitter);
	return cookedLight;
}

EmitterFeatureSet AGeometricLight::getEmitterFeatureSet() const
{
	EmitterFeatureSet featureSet = Emitter::defaultFeatureSet;

	featureSet.turnOff({EEmitterFeatureSet::ZeroBounceSample});
	if(m_isDirectlyVisible)
	{
		featureSet.turnOn({EEmitterFeatureSet::ZeroBounceSample});
	}

	featureSet.turnOff({EEmitterFeatureSet::BsdfSample});
	if(m_useBsdfSample)
	{
		featureSet.turnOn({EEmitterFeatureSet::BsdfSample});
	}

	featureSet.turnOff({EEmitterFeatureSet::DirectSample});
	if(m_useDirectSample)
	{
		featureSet.turnOn({EEmitterFeatureSet::DirectSample});
	}

	featureSet.turnOff({EEmitterFeatureSet::EmissionSample});
	if(m_useEmissionSample)
	{
		featureSet.turnOn({EEmitterFeatureSet::EmissionSample});
	}

	return featureSet;
}

const CookedGeometry* AGeometricLight::getSanifiedGeometry(
	const std::shared_ptr<Geometry>& srcGeometry,
	const TransformInfo& srcLocalToWorld,
	const CookingContext& ctx)
{
	if(!srcGeometry)
	{
		return nullptr;
	}

	GeometryCookingConfig geometryConfig = ctx.getGeometryConfig();

	// TODO: test "isRigid()" may be more appropriate
	if(srcLocalToWorld.getDecomposed().hasScaleEffect())
	{
		PH_LOG(AGeometricLight, Note,
			"scale detected (which is {}), this is undesirable since many light attributes will "
			"be affected; baking the full transform can incur additional memory overhead as a "
			"separate cooked geometry variant may be required",
			srcLocalToWorld.getScale());

		// Bake the full transform so light sampling uses the correct geometry surface area.
		geometryConfig.forceBakedTransform = true;
		geometryConfig.bakedTransform = srcLocalToWorld.getDecomposed();
	}

	const CookingContext geometryCtx = ctx.withGeometryConfig(geometryConfig);
	const auto key = geometryCtx.getKey(srcGeometry);
	if(const CookedGeometry* cookedGeometry = geometryCtx.getResources().getGeometry(key))
	{
		return cookedGeometry;
	}

	CookedGeometry cookedGeometry;
	srcGeometry->cook(geometryCtx, cookedGeometry);
	return geometryCtx.getResources().makeGeometry(key, std::move(cookedGeometry));
}

}// end namespace ph
