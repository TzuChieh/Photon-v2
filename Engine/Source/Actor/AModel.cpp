#include "Actor/AModel.h"
#include "Math/math.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "World/Foundation/CookedUnit.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Core/Intersectable/TransformedIntersectable.h"
#include "Core/Quantity/Time.h"
#include "Actor/ModelBuilder.h"
#include "World/Foundation/PreCookReport.h"
#include "World/Foundation/CookingContext.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "Common/logging.h"

#include <algorithm>
#include <iostream>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(AModel, Actor);

PreCookReport AModel::preCook(CookingContext& ctx)
{
	PreCookReport report = PhysicalActor::preCook(ctx);

	auto localToWorld = ctx.getCooked()->makeTransform<math::StaticAffineTransform>(
		math::StaticAffineTransform::makeForward(m_localToWorld));
	auto worldToLocal = ctx.getCooked()->makeTransform<math::StaticAffineTransform>(
		math::StaticAffineTransform::makeInverse(m_localToWorld));

	report.setBaseTransforms(localToWorld, worldToLocal);

	return report;
}

CookedUnit AModel::cook(CookingContext& ctx, const PreCookReport& report)
{
	if(!m_geometry || !m_material)
	{
		PH_LOG_WARNING(AModel,
			"incomplete data detected (missing geometry: {}, missing material: {})",
			m_geometry == nullptr, m_material == nullptr);

		return CookedUnit();
	}

	ModelBuilder builder(ctx);
	
	PrimitiveMetadata* metadata = ctx.getCooked()->makeMetadata();

	PrimitiveBuildingMaterial primitiveBuildingMatl(metadata);

	std::vector<std::unique_ptr<Primitive>> primitives;
	m_geometry->genPrimitive(primitiveBuildingMatl, primitives);
	for(auto& primitive : primitives)
	{
		builder.addIntersectable(std::move(primitive));
	}

	m_material->genBehaviors(ctx, *metadata);

	auto baseLW = std::make_unique<math::StaticAffineTransform>(math::StaticAffineTransform::makeForward(m_localToWorld));
	auto baseWL = std::make_unique<math::StaticAffineTransform>(math::StaticAffineTransform::makeInverse(m_localToWorld));
	builder.transform(std::move(baseLW), std::move(baseWL));
	
	if(m_motionSource)
	{
		// HACK
		Time t0;
		Time t1;
		t1.absoluteS = 1;
		t1.relativeS = 1;
		t1.relativeT = 1;

		auto motionLW = m_motionSource->genLocalToWorld(t0, t1);
		auto motionWL = motionLW->genInversed();
		builder.transform(std::move(motionLW), std::move(motionWL));
	}

	return builder.claimBuildResult();
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
