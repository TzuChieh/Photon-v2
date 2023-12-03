#include "Actor/AModel.h"
#include "Math/math.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "World/Foundation/TransientVisualElement.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Core/Intersectable/TransformedIntersectable.h"
#include "Core/Quantity/Time.h"
#include "Actor/ModelBuilder.h"
#include "World/Foundation/PreCookReport.h"
#include "World/Foundation/CookingContext.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "World/Foundation/CookedGeometry.h"
#include "World/Foundation/CookedMotion.h"
#include "Common/logging.h"
#include "Core/Intersectable/TMetaInjectionPrimitive.h"

#include <algorithm>
#include <iostream>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(AModel, Actor);

PreCookReport AModel::preCook(const CookingContext& ctx) const
{
	PreCookReport report = PhysicalActor::preCook(ctx);

	if(!m_localToWorld.getDecomposed().isIdentity())
	{
		auto localToWorld = ctx.getResources()->makeTransform<math::StaticAffineTransform>(
			m_localToWorld.getForwardStaticAffine());
		auto worldToLocal = ctx.getResources()->makeTransform<math::StaticAffineTransform>(
			m_localToWorld.getInverseStaticAffine());

		report.setBaseTransforms(localToWorld, worldToLocal);
	}

	return report;
}

TransientVisualElement AModel::cook(const CookingContext& ctx, const PreCookReport& report) const
{
	if(!m_geometry || !m_material)
	{
		PH_LOG_WARNING(AModel,
			"incomplete data detected (missing geometry: {}, missing material: {})",
			m_geometry == nullptr, m_material == nullptr);

		return TransientVisualElement();
	}
	
	PrimitiveMetadata* metadata = ctx.getResources()->makeMetadata();
	// FIXME
	const CookedGeometry* cookedGeometry = m_geometry->createCooked(ctx);

	TransientVisualElement result;
	for(const Primitive* primitive : cookedGeometry->primitives)
	{
		auto* metaPrimitive = ctx.getResources()->copyIntersectable(TMetaInjectionPrimitive(
			ReferencedPrimitiveMetaGetter(metadata),
			TReferencedPrimitiveGetter<Primitive>(primitive)));

		result.add(metaPrimitive);
	}
	
	if(!m_localToWorld.getDecomposed().isIdentity())
	{
		// Cannot have primitive view as we are transforming as intersectable
		result.primitivesView.clear();

		auto localToWorld = report.getBaseLocalToWorld();
		auto worldToLocal = report.getBaseWorldToLocal();

		for(auto& intersectable : result.intersectables)
		{
			auto* transformedIntersectable = ctx.getResources()->makeIntersectable<TransformedIntersectable>(
				intersectable, localToWorld, worldToLocal);

			intersectable = transformedIntersectable;
		}
	}

	if(m_motionSource)
	{
		// Cannot have primitive view as we are transforming as intersectable
		result.primitivesView.clear();

		// FIXME
		const CookedMotion* cookedMotion = m_motionSource->createCooked(ctx, MotionCookConfig());

		auto localToWorld = cookedMotion->localToWorld;
		auto worldToLocal = cookedMotion->worldToLocal;

		for(auto& intersectable : result.intersectables)
		{
			auto* transformedIntersectable = ctx.getResources()->makeIntersectable<TransformedIntersectable>(
				intersectable, localToWorld, worldToLocal);

			intersectable = transformedIntersectable;
		}
	}

	m_material->genBehaviors(ctx, *metadata);

	return result;
}

void AModel::setGeometry(const std::shared_ptr<Geometry>& geometry)
{
	m_geometry = geometry;
}

void AModel::setMaterial(const std::shared_ptr<Material>& material)
{
	m_material = material;
}

void AModel::setMotionSource(const std::shared_ptr<MotionSource>& motion)
{
	m_motionSource = motion;
}

const Geometry* AModel::getGeometry() const
{
	return m_geometry.get();
}

const Material* AModel::getMaterial() const
{
	return m_material.get();
}

}// end namespace ph
