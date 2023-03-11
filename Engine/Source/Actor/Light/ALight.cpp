#include "Actor/Light/ALight.h"
#include "Math/math.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Material/MatteOpaque.h"
#include "World/Foundation/TransientVisualElement.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "Core/Intersectable/TransformedIntersectable.h"
#include "Actor/ModelBuilder.h"
#include "Core/Intersectable/TransformedPrimitive.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Math/Transform/StaticRigidTransform.h"
#include "World/Foundation/PreCookReport.h"
#include "World/Foundation/CookingContext.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "Common/logging.h"
#include "Core/Intersectable/TMetaInjectionPrimitive.h"

#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(ALight, Actor);

PreCookReport ALight::preCook(CookingContext& ctx)
{
	PreCookReport report = PhysicalActor::preCook(ctx);

	if(isGeometric())
	{
		// TODO: test "isRigid()" may be more appropriate
		if(m_localToWorld.hasScaleEffect() || m_localToWorld.isIdentity())
		{
			report.setBaseTransforms(nullptr, nullptr);
		}
		else
		{
			auto localToWorld = ctx.getResources()->makeTransform<math::StaticRigidTransform>(
				math::StaticRigidTransform::makeForward(m_localToWorld));
			auto worldToLocal = ctx.getResources()->makeTransform<math::StaticRigidTransform>(
				math::StaticRigidTransform::makeInverse(m_localToWorld));

			report.setBaseTransforms(localToWorld, worldToLocal);
		}
	}

	return report;
}

TransientVisualElement ALight::cook(CookingContext& ctx, const PreCookReport& report)
{
	if(!m_lightSource)
	{
		PH_LOG_WARNING(ALight, "incomplete data detected, this light is ignored");
		return TransientVisualElement();
	}

	PH_ASSERT(m_lightSource);
	std::shared_ptr<Geometry> geometry = m_lightSource->genGeometry(ctx);

	TransientVisualElement cookedActor;
	if(geometry)
	{
		std::shared_ptr<Material> material = m_lightSource->genMaterial(ctx);
		cookedActor = buildGeometricLight(ctx, geometry, material, report);
	}
	else
	{
		std::unique_ptr<Emitter> emitter = m_lightSource->genEmitter(ctx, EmitterBuildingMaterial());
		cookedActor.emitter = std::move(emitter);
	}

	return cookedActor;
}

const LightSource* ALight::getLightSource() const
{
	return m_lightSource.get();
}

void ALight::setLightSource(const std::shared_ptr<LightSource>& lightSource)
{
	m_lightSource = lightSource;
}

TransientVisualElement ALight::buildGeometricLight(
	CookingContext& ctx,
	const std::shared_ptr<Geometry>& srcGeometry,
	const std::shared_ptr<Material>& srcMaterial,
	const PreCookReport& report) const
{
	std::shared_ptr<Geometry> geometry = srcGeometry;
	std::shared_ptr<Material> material = srcMaterial;

	if(!isGeometric() || !geometry)
	{
		PH_LOG_ERROR(ALight, 
			"cannot build geometric light, please make sure the actor is geometric or supply a "
			"valid geometry resource");

		return TransientVisualElement();
	}

	if(!material)
	{
		PH_LOG(ALight, 
			"material is not specified, using default diffusive material");

		material = std::make_shared<MatteOpaque>();
	}

	math::TDecomposedTransform<real> remainingLocalToWorld;
	auto sanifiedGeometry = getSanifiedGeometry(geometry, &remainingLocalToWorld);

	PrimitiveMetadata* metadata = ctx.getResources()->makeMetadata();
	material->genBehaviors(ctx, *metadata);

	// FIXME
	const CookedGeometry* cookedGeometry = sanifiedGeometry->createCooked(ctx, GeometryCookConfig());

	std::vector<const Primitive*> lightPrimitives;
	lightPrimitives.reserve(cookedGeometry->primitives.size());
	for(const Primitive* primitive : cookedGeometry->primitives)
	{
		auto* metaPrimitive = ctx.getResources()->copyIntersectable(TMetaInjectionPrimitive(
			ReferencedPrimitiveMetaGetter(metadata),
			TReferencedPrimitiveGetter<Primitive>(primitive)));

		lightPrimitives.push_back(metaPrimitive);
	}

	if(m_localToWorld.isIdentity())
	{
		// Just to make sure we are not pre-cooking identity transforms
		PH_ASSERT(!report.getBaseLocalToWorld());
		PH_ASSERT(!report.getBaseWorldToLocal());
	}
	else
	{
		const math::RigidTransform* localToWorld = nullptr;
		const math::RigidTransform* worldToLocal = nullptr;
		if(m_localToWorld.hasScaleEffect())
		{
			// Should use transform from the sanification process (should not be pre-cooked)
			PH_ASSERT(!report.getBaseLocalToWorld());
			PH_ASSERT(!report.getBaseWorldToLocal());

			if(!remainingLocalToWorld.isIdentity())
			{
				localToWorld = ctx.getResources()->makeTransform<math::StaticRigidTransform>(
					math::StaticRigidTransform::makeForward(remainingLocalToWorld));
				worldToLocal = ctx.getResources()->makeTransform<math::StaticRigidTransform>(
					math::StaticRigidTransform::makeInverse(remainingLocalToWorld));
			}
		}
		else
		{
			// Can (and should) be pre-cooked
			PH_ASSERT(report.getBaseLocalToWorld());
			PH_ASSERT(report.getBaseWorldToLocal());

			// Must match the type used in `preCook()`
			localToWorld = static_cast<const math::StaticRigidTransform*>(report.getBaseLocalToWorld());
			worldToLocal = static_cast<const math::StaticRigidTransform*>(report.getBaseWorldToLocal());
		}

		if(localToWorld && worldToLocal)
		{
			for(auto& lightPrimitive : lightPrimitives)
			{
				auto* transformedPrimitive = ctx.getResources()->makeIntersectable<TransformedPrimitive>(
					lightPrimitive, localToWorld, worldToLocal);

				lightPrimitive = transformedPrimitive;
			}
		}
	}

	TransientVisualElement cookedLight;
	for(const Primitive* primitive : lightPrimitives)
	{
		cookedLight.intersectables.push_back(primitive);
	}

	EmitterBuildingMaterial emitterBuildingMaterial;
	emitterBuildingMaterial.primitives = lightPrimitives;
	emitterBuildingMaterial.metadata   = metadata;
	auto emitter = m_lightSource->genEmitter(ctx, std::move(emitterBuildingMaterial));

	metadata->getSurface().setEmitter(emitter.get());
	cookedLight.emitter = std::move(emitter);

	return cookedLight;
}

std::shared_ptr<Geometry> ALight::getSanifiedGeometry(
	const std::shared_ptr<Geometry>& srcGeometry,
	math::TDecomposedTransform<real>* const out_remainingLocalToWorld) const
{
	if(!srcGeometry)
	{
		return nullptr;
	}

	std::shared_ptr<Geometry> sanifiedGeometry = nullptr;

	// TODO: test "isRigid()" may be more appropriate
	if(m_localToWorld.hasScaleEffect())
	{
		PH_LOG(ALight,
			"scale detected, this is undesirable since many light attributes will "
			"be affected; can incur additional memory overhead as the original cooked "
			"geometry may not be used (e.g., a transformed temporary is used instead "
			"and the original is not referenced)");

		const auto baseLW = math::StaticAffineTransform::makeForward(m_localToWorld);

		sanifiedGeometry = srcGeometry->genTransformed(baseLW);
		if(!sanifiedGeometry)
		{
			PH_LOG_WARNING(ALight,
				"scale detected and has failed to apply it to the geometry; "
				"scaling on light with attached geometry may have unexpected "
				"behaviors such as miscalculated primitive surface area, which "
				"can cause severe rendering artifacts");

			sanifiedGeometry = srcGeometry;
			
			if(out_remainingLocalToWorld)
			{
				*out_remainingLocalToWorld = m_localToWorld;
			}
		}
		else
		{
			*out_remainingLocalToWorld = math::TDecomposedTransform<real>();
		}
	}
	else
	{
		sanifiedGeometry = srcGeometry;

		if(out_remainingLocalToWorld)
		{
			*out_remainingLocalToWorld = m_localToWorld;
		}
	}

	return sanifiedGeometry;
}

}// end namespace ph
