#include "Actor/ADome.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Material/IdealSubstance.h"
#include "World/Foundation/CookingContext.h"
#include "Math/Transform/StaticRigidTransform.h"
#include "Core/Intersection/PLatLongEnvSphere.h"
#include "Core/Emitter/LatLongEnvEmitter.h"
#include "Actor/Dome/AImageDome.h"
#include "Actor/Dome/APreethamDome.h"
#include "Core/Texture/Function/unary_texture_operators.h"
#include "World/Foundation/CookOrder.h"
#include "World/Foundation/PreCookReport.h"
#include "World/Foundation/CookingContext.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "Core/Intersection/TMetaInjectionPrimitive.h"

#include <Common/logging.h>

#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(DomeActor, Actor);

PreCookReport ADome::preCook(const CookingContext& ctx) const
{
	PreCookReport report = PhysicalActor::preCook(ctx);

	// Ensure reasonable transformation for the dome
	math::TDecomposedTransform<real> sanifiedLocalToWorld = m_localToWorld.getDecomposed();
	if(sanifiedLocalToWorld.hasScaleEffect())
	{
		PH_LOG(DomeActor, Warning,
			"Scale detected and is ignored; scaling on dome light should be avoided as it does "
			"not have any effect. If resizing the dome is desired, it should be done by "
			"changing its radius.");

		sanifiedLocalToWorld.setScale(1);
	}

	auto localToWorld = ctx.getResources()->makeTransform<math::StaticRigidTransform>(
		math::StaticRigidTransform::makeForward(sanifiedLocalToWorld));
	auto worldToLocal = ctx.getResources()->makeTransform<math::StaticRigidTransform>(
		math::StaticRigidTransform::makeInverse(sanifiedLocalToWorld));

	report.setBaseTransforms(localToWorld, worldToLocal);

	return report;
}

TransientVisualElement ADome::cook(const CookingContext& ctx, const PreCookReport& report) const
{
	// Must match what was used in `preCook()`
	auto localToWorld = static_cast<const math::StaticRigidTransform*>(report.getBaseLocalToWorld());
	auto worldToLocal = static_cast<const math::StaticRigidTransform*>(report.getBaseWorldToLocal());

	const math::AABB3D worldBound = ctx.getLeafActorsBound();
	
	math::Vector3R domeCenter;
	localToWorld->transformP({0, 0, 0}, &domeCenter);

	// Get the sphere radius that can encompass all actors
	real domeRadius = 1.0_r;
	for(auto vertex : worldBound.getBoundVertices())
	{
		constexpr auto enlargement = 1.01_r;

		const auto centerToVertex = vertex - domeCenter;
		const auto ri = centerToVertex.length() * enlargement;

		domeRadius = std::max(ri, domeRadius);
	}

	PrimitiveMetadata* metadata = ctx.getResources()->makeMetadata();

	// A dome should not have any visible inter-reflections, ideally
	auto material = std::make_shared<IdealSubstance>();
	material->setSubstance(EIdealSubstance::Absorber);
	material->genBehaviors(ctx, *metadata);

	auto* domePrimitive = ctx.getResources()->copyIntersectable(TMetaInjectionPrimitive(
		ReferencedPrimitiveMetaGetter(metadata), 
		TEmbeddedPrimitiveGetter<PLatLongEnvSphere>(domeRadius, localToWorld, worldToLocal)));
	
	DomeRadianceFunctionInfo radianceFunctionInfo;
	auto radianceFunction = loadRadianceFunction(ctx, &radianceFunctionInfo);
	if(m_energyScale != 1.0_r)
	{
		auto scaledRadianceFunction = std::make_shared<TUnaryTextureOperator<math::Spectrum, math::Spectrum, texfunc::SpectrumMultiplyScalar>>(
			radianceFunction, texfunc::SpectrumMultiplyScalar(static_cast<math::ColorValue>(m_energyScale)));

		radianceFunction = scaledRadianceFunction;
	}

	const Emitter* domeEmitter = nullptr;
	if(!radianceFunctionInfo.isAnalytical)
	{
		domeEmitter = ctx.getResources()->makeEmitter<LatLongEnvEmitter>(
			domePrimitive,
			radianceFunction,
			radianceFunctionInfo.resolution);
	}
	else
	{
		// FIXME: proper resolution for analytical emitter
		domeEmitter = ctx.getResources()->makeEmitter<LatLongEnvEmitter>(
			domePrimitive,
			radianceFunction,
			math::Vector2S(512, 256));
	}

	metadata->getSurface().setEmitter(domeEmitter);
	
	// Store cooked data

	ctx.getResources()->getNamed()->setBackgroundPrimitive(domePrimitive);

	TransientVisualElement result;
	result.emitters.push_back(domeEmitter);
	return result;
}

CookOrder ADome::getCookOrder() const
{
	return CookOrder(ECookPriority::Low, ECookLevel::Last);
}

}// end namespace ph
