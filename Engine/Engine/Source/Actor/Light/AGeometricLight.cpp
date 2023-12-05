#include "Actor/Light/AGeometricLight.h"
#include "Math/math.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Material/MatteOpaque.h"
#include "World/Foundation/TransientVisualElement.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "Core/Intersectable/TransformedIntersectable.h"
#include "Core/Intersectable/TransformedPrimitive.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Math/Transform/StaticRigidTransform.h"
#include "World/Foundation/PreCookReport.h"
#include "World/Foundation/CookingContext.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "Core/Intersectable/TMetaInjectionPrimitive.h"
#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Core/Emitter/MultiDiffuseSurfaceEmitter.h"

#include <Common/logging.h>

#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(AGeometricLight, Actor);

std::shared_ptr<Material> AGeometricLight::getMaterial(const CookingContext& ctx) const
{
	return TSdl<MatteOpaque>::makeResource();
}

PreCookReport AGeometricLight::preCook(const CookingContext& ctx) const
{
	PreCookReport report = PhysicalActor::preCook(ctx);

	// TODO: test "isRigid()" may be more appropriate
	if(m_localToWorld.getDecomposed().hasScaleEffect() || m_localToWorld.getDecomposed().isIdentity())
	{
		report.setBaseTransforms(nullptr, nullptr);
	}
	else
	{
		auto* localToWorld = ctx.getResources()->makeTransform<math::StaticRigidTransform>(
			m_localToWorld.getForwardStaticRigid());
		auto* worldToLocal = ctx.getResources()->makeTransform<math::StaticRigidTransform>(
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
		PH_LOG_ERROR(AGeometricLight,
			"cannot build geometric light, please make sure the actor is geometric or supply a "
			"valid geometry resource");
		return TransientVisualElement();
	}

	if(!material)
	{
		PH_LOG(AGeometricLight,
			"material is not specified, using default diffusive material");
		material = TSdl<MatteOpaque>::makeResource();
	}

	math::TDecomposedTransform<real> remainingLocalToWorld;
	auto sanifiedGeometry = getSanifiedGeometry(geometry, m_localToWorld, &remainingLocalToWorld);

	PrimitiveMetadata* metadata = ctx.getResources()->makeMetadata();
	material->genBehaviors(ctx, *metadata);

	// FIXME
	const CookedGeometry* cookedGeometry = sanifiedGeometry->createCooked(ctx);

	std::vector<const Primitive*> lightPrimitives;
	lightPrimitives.reserve(cookedGeometry->primitives.size());
	for(const Primitive* primitive : cookedGeometry->primitives)
	{
		auto* metaPrimitive = ctx.getResources()->copyIntersectable(TMetaInjectionPrimitive(
			ReferencedPrimitiveMetaGetter(metadata),
			TReferencedPrimitiveGetter<Primitive>(primitive)));

		lightPrimitives.push_back(metaPrimitive);
	}

	if(m_localToWorld.getDecomposed().isIdentity())
	{
		// Just to make sure we are not pre-cooking identity transforms
		PH_ASSERT(!report.getBaseLocalToWorld());
		PH_ASSERT(!report.getBaseWorldToLocal());
	}
	else
	{
		const math::RigidTransform* localToWorld = nullptr;
		const math::RigidTransform* worldToLocal = nullptr;
		if(m_localToWorld.getDecomposed().hasScaleEffect())
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
		cookedLight.add(primitive);
	}

	const Emitter* emitter = buildEmitter(ctx, lightPrimitives);
	if(!emitter)
	{
		PH_LOG_ERROR(AGeometricLight,
			"no emitter generated");
		return cookedLight;
	}

	cookedLight.emitters.push_back(emitter);
	metadata->getSurface().setEmitter(emitter);
	return cookedLight;
}

std::shared_ptr<Geometry> AGeometricLight::getSanifiedGeometry(
	const std::shared_ptr<Geometry>& srcGeometry,
	const TransformInfo& srcLocalToWorld,
	math::TDecomposedTransform<real>* const out_remainingLocalToWorld)
{
	if(!srcGeometry)
	{
		return nullptr;
	}

	std::shared_ptr<Geometry> sanifiedGeometry = nullptr;

	// TODO: test "isRigid()" may be more appropriate
	if(srcLocalToWorld.getDecomposed().hasScaleEffect())
	{
		PH_LOG(AGeometricLight,
			"scale detected (which is {}), this is undesirable since many light attributes will "
			"be affected; can incur additional memory overhead as the original cooked geometry "
			"may not be used (e.g., a transformed temporary is used instead and the original is "
			"not referenced)",
			srcLocalToWorld.getScale());

		const auto baseLW = srcLocalToWorld.getForwardStaticAffine();

		sanifiedGeometry = srcGeometry->genTransformed(baseLW);
		if(!sanifiedGeometry)
		{
			PH_LOG_WARNING(AGeometricLight,
				"scale detected and has failed to apply it to the geometry; "
				"scaling on light with attached geometry may have unexpected "
				"behaviors such as miscalculated primitive surface area, which "
				"can cause severe rendering artifacts");

			sanifiedGeometry = srcGeometry;
			
			if(out_remainingLocalToWorld)
			{
				*out_remainingLocalToWorld = srcLocalToWorld.getDecomposed();
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
			*out_remainingLocalToWorld = srcLocalToWorld.getDecomposed();
		}
	}

	return sanifiedGeometry;
}

}// end namespace ph
