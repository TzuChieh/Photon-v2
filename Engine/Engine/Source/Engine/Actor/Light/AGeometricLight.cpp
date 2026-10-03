#include "Engine/Actor/Light/AGeometricLight.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/Math/math.h"
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
	return nullptr;
}

void AGeometricLight::setIsIntersectable(const bool isIntersectable)
{
	m_isIntersectable = isIntersectable;
}

bool AGeometricLight::isIntersectable() const
{
	return m_isIntersectable;
}

void AGeometricLight::setIsDirectlyVisible(const bool isDirectlyVisible)
{
	m_isDirectlyVisible = isDirectlyVisible;
}

void AGeometricLight::setEmitBackward(const bool shouldEmitBackward)
{
	m_shouldEmitBackward = shouldEmitBackward;
}

bool AGeometricLight::shouldEmitBackward() const
{
	return m_shouldEmitBackward;
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
	const CookedGeometry* cookedGeometry = getSanifiedGeometry(ctx);
	if(!cookedGeometry)
	{
		throw ActorCookException(
			"cannot build geometric light, please make sure the actor is geometric or supply a "
			"valid geometry resource");
	}

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

	std::shared_ptr<Material> material;
	const CookedMaterial* cookedMaterial = getSanifiedMaterial(ctx, &material);
	if(cookedMaterial)
	{
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

	SurfaceEmitter* surfaceEmitter = buildSurfaceEmitter(ctx, lightPrimitives);
	if(!surfaceEmitter)
	{
		PH_LOG(AGeometricLight, Error, "no emitter generated");
		return {};
	}

	TransientVisualElement cookedLight;

	if(cookedMaterial)
	{
		for(const Primitive* primitive : lightPrimitives)
		{
			cookedLight.add(primitive);
		}
	}
	// A light without a physical material is non-blocking
	else
	{
		for(const Primitive* primitive : lightPrimitives)
		{
			cookedLight.addNonBlockingEmitterPrimitive(primitive);
		}
	}

	if(m_shouldEmitBackward)
	{
		surfaceEmitter->setBackFaceEmit();
	}
	else
	{
		surfaceEmitter->setFrontFaceEmit();
	}

	const bool isNonPhysical =
		!cookedMaterial ||
		surfaceEmitter->getFeatureSet().hasNo(EEmitterFeatureSet::ZeroBounceSample);
	cookedLight.surfaceEmitters.push_back({surfaceEmitter, isNonPhysical});
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
	const CookingContext& ctx,
	std::shared_ptr<Geometry>* const out_geometryResource) const
{
	GeometryCookingConfig geometryConfig = ctx.getGeometryConfig();

	// TODO: test "isRigid()" may be more appropriate
	if(m_localToWorld.getDecomposed().hasScaleEffect())
	{
		PH_LOG(AGeometricLight, Note,
			"scale detected (which is {}), this is undesirable since many light attributes will "
			"be affected; baking the full transform can incur additional memory overhead as a "
			"separate cooked geometry variant may be required",
			m_localToWorld.getScale());

		// Bake the full transform so light sampling uses the correct geometry surface area.
		geometryConfig.forceBakedTransform = true;
		geometryConfig.bakedTransform = m_localToWorld.getDecomposed();
	}

	const CookingContext geometryCtx = ctx.withGeometryConfig(geometryConfig);
	const std::shared_ptr<Geometry> geometry = getGeometry(geometryCtx);
	if(out_geometryResource)
	{
		*out_geometryResource = geometry;
	}
	if(!geometry)
	{
		return nullptr;
	}

	const auto key = geometryCtx.getKey(geometry);
	if(const CookedGeometry* cookedGeometry = geometryCtx.getResources().getGeometry(key))
	{
		return cookedGeometry;
	}

	CookedGeometry cookedGeometry;
	geometry->cook(geometryCtx, cookedGeometry);
	return geometryCtx.getResources().makeGeometry(key, std::move(cookedGeometry));
}

const CookedMaterial* AGeometricLight::getSanifiedMaterial(
	const CookingContext& ctx,
	std::shared_ptr<Material>* const out_materialResource) const
{
	if(out_materialResource)
	{
		*out_materialResource = nullptr;
	}

	if(!isIntersectable())
	{
		return nullptr;
	}

	const CookingContext materialCtx = ctx.withMaterialConfig(ctx.getMaterialConfig());
	const std::shared_ptr<Material> material = getMaterial(materialCtx);
	if(out_materialResource)
	{
		*out_materialResource = material;
	}
	if(!material)
	{
		return nullptr;
	}

	const auto key = materialCtx.getKey(material);
	if(const CookedMaterial* cookedMaterial = materialCtx.getResources().getMaterial(key))
	{
		return cookedMaterial;
	}

	CookedMaterial cookedMaterial;
	material->cook(materialCtx, cookedMaterial);
	return materialCtx.getResources().makeMaterial(key, std::move(cookedMaterial));
}

}// end namespace ph
