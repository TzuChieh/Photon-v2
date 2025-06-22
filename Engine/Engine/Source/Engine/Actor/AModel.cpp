#include "Engine/Actor/AModel.h"
#include "Engine/Math/math.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/Intersection/TransformedIntersectable.h"
#include "Engine/Core/Intersection/TMetaInjectionPrimitive.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/World/Foundation/TransientVisualElement.h"
#include "Engine/Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Engine/Core/Transform/StaticAffineTransform.h"
#include "Engine/Core/Quantity/Time.h"
#include "Engine/Actor/ModelBuilder.h"
#include "Engine/World/Foundation/PreCookReport.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/World/Foundation/CookedGeometry.h"
#include "Engine/World/Foundation/CookedMotion.h"

#include <Common/logging.h>

#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(AModel, Actor);

PreCookReport AModel::preCook(const CookingContext& ctx) const
{
	PreCookReport report = PhysicalActor::preCook(ctx);

	if(!m_localToWorld.getDecomposed().isIdentity())
	{
		auto localToWorld = ctx.getResources().makeTransform<StaticAffineTransform>(
			m_localToWorld.getForwardStaticAffine());
		auto worldToLocal = ctx.getResources().makeTransform<StaticAffineTransform>(
			m_localToWorld.getInverseStaticAffine());

		report.setBaseTransforms(localToWorld, worldToLocal);
	}

	return report;
}

TransientVisualElement AModel::cook(const CookingContext& ctx, const PreCookReport& report) const
{
	if(!m_geometry || !m_material)
	{
		PH_LOG(AModel, Warning,
			"incomplete data detected (missing geometry: {}, missing material: {})",
			m_geometry == nullptr, m_material == nullptr);

		return TransientVisualElement();
	}
	
	PrimitiveMetadata* metadata = ctx.getResources().makeMetadata();
	// FIXME
	const CookedGeometry* cookedGeometry = m_geometry->createCooked(ctx);

	TransientVisualElement result;
	for(const Primitive* primitive : cookedGeometry->primitives)
	{
		auto* metaPrimitive = ctx.getResources().copyIntersectable(TMetaInjectionPrimitive(
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
			auto* transformedIntersectable = ctx.getResources().makeIntersectable<TransformedIntersectable>(
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
			auto* transformedIntersectable = ctx.getResources().makeIntersectable<TransformedIntersectable>(
				intersectable, localToWorld, worldToLocal);

			intersectable = transformedIntersectable;
		}
	}

	CookedMaterial* cookedMaterial = m_material->createCooked(ctx);
	metadata->surface().setOptics(cookedMaterial->surfaceOptics);

	if(m_material->getOverlapPriority() > 0)
	{
		const VolumeOptics* interiorOptics = nullptr;
		const VolumeOptics* exteriorOptics = nullptr;
		cookedMaterial->findFirstCompatibleOptics(&interiorOptics, &exteriorOptics);

		metadata->interior().setOptics(interiorOptics);
		metadata->exterior().setOptics(exteriorOptics);
		metadata->setInteriorPriority(m_material->getOverlapPriority());
	}

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
