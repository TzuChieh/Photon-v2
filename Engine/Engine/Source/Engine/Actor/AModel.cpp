#include "Engine/Actor/AModel.h"
#include "Engine/Math/math.h"
#include "Engine/Core/Intersection/BVH/TBinaryBvhIntersector.h"
#include "Engine/Core/Intersection/IntersectableBuilder.h"
#include "Engine/Core/Intersection/TMaskedIntersectable.h"
#include "Engine/Core/Intersection/PrimitiveBuilder.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/World/Foundation/TransientVisualElement.h"
#include "Engine/Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Engine/Core/Transform/StaticAffineTransform.h"
#include "Engine/Actor/ModelBuilder.h"
#include "Engine/World/Foundation/PreCookReport.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/World/Foundation/CookedGeometry.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookedMotion.h"

#include <Common/logging.h>

#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(AModel, Actor);

PreCookReport AModel::preCook(const CookingContext& ctx) const
{
	PreCookReport report = PhysicalActor::preCook(ctx);

	if(!m_geometry || !m_material)
	{
		PH_LOG(AModel, Warning,
			"incomplete data detected (missing geometry: {}, missing material: {})",
			m_geometry == nullptr, m_material == nullptr);

		return report.markAsUncookable();
	}

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
	const CookedGeometry* cookedGeometry = ctx.getCooked(m_geometry);
	if(cookedGeometry->primitives.empty())
	{
		return TransientVisualElement();
	}

	// Our default policy is Ng facing is unaffectd by winding change, 
	// so if `isWindingFlipped` is true then that implies a flip, if `m_shouldFlipNg`
	// is specified additionally then they can cancel out
	const bool shouldFlipNg = m_shouldFlipNg != cookedGeometry->isWindingFlipped;

	const CookedMaterial* cookedMaterial = ctx.getCooked(m_material);
	PrimitiveMetadata* metadata = ctx.getResources().makeMetadata();
	metadata->setGeometryInfo(&cookedGeometry->geometryInfo);

	const auto* const localToWorld = report.getBaseLocalToWorld();
	const auto* const worldToLocal = report.getBaseWorldToLocal();

	TransientVisualElement result;
	for(const Primitive* primitive : cookedGeometry->primitives)
	{
		auto primitiveBuilder = PrimitiveBuilder::referencing(primitive)
			.injectMetadata(metadata);

		// Have transform
		if(localToWorld)
		{
			PH_ASSERT(worldToLocal);
			if(shouldFlipNg)
			{
				result.intersectables.push_back(
					ctx.getResources().copyIntersectable(
						primitiveBuilder.transform<true>(localToWorld, worldToLocal).build()));
			}
			else
			{
				result.intersectables.push_back(
					ctx.getResources().copyIntersectable(
						primitiveBuilder.transform(localToWorld, worldToLocal).build()));
			}
		}
		// No transform
		else
		{
			if(shouldFlipNg)
			{
				result.add(
					ctx.getResources().copyIntersectable(
						primitiveBuilder.flipGeometryNormal().build()));
			}
			else
			{
				result.add(
					ctx.getResources().copyIntersectable(
						primitiveBuilder.build()));
			}
		}
	}

	if(m_motionSource)
	{
		// Cannot have primitive view as we are transforming as intersectable
		result.primitivesView.clear();

		const CookedMotion* cookedMotion = ctx.getCooked(m_motionSource);

		auto localToWorld = cookedMotion->localToWorld;
		auto worldToLocal = cookedMotion->worldToLocal;

		for(auto& intersectable : result.intersectables)
		{
			auto* transformedIntersectable = ctx.getResources().copyIntersectable(
				IntersectableBuilder::referencing(intersectable)
					.transform(localToWorld, worldToLocal)
					.build());

			intersectable = transformedIntersectable;
		}
	}

	metadata->surface().setOptics(cookedMaterial->surfaceOptics);
	metadata->setInterfaceMask(cookedMaterial->interfaceMask.get());

	if(m_material->getOverlapPriority() > 0)
	{
		const VolumeOptics* interiorOptics = nullptr;
		const VolumeOptics* exteriorOptics = nullptr;
		cookedMaterial->findFirstCompatibleOptics(&interiorOptics, &exteriorOptics);

		metadata->interior().setOptics(interiorOptics);
		metadata->exterior().setOptics(exteriorOptics);
		metadata->setInteriorPriority(m_material->getOverlapPriority());
	}

	if(cookedMaterial->interfaceMask)
	{
		// Masking will cause mismatched view and intersectables
		result.primitivesView.clear();

		for(auto& intersectable : result.intersectables)
		{
			intersectable = ctx.getResources().makeIntersectable<MaterialMaskedIntersectable>(
				intersectable, MaterialInterfaceMask{});
		}
	}

	if(isInstantiableHint() && result.intersectables.size() > 1)
	{
		auto* aggregate = ctx.getResources().makeIntersectable<TBinaryBvhIntersector<uint32>>();
		aggregate->update(result.intersectables);
		result.intersectables = {aggregate};
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

void AModel::setShouldFlipNg(const bool shouldFlipNg)
{
	m_shouldFlipNg = shouldFlipNg;
}

bool AModel::shouldFlipNg() const
{
	return m_shouldFlipNg;
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
