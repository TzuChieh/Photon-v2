#include "Engine/Actor/ADome.h"
#include "Engine/Actor/Geometry/GSphere.h"
#include "Engine/Actor/Material/IdealSubstance.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/Core/Transform/StaticRigidTransform.h"
#include "Engine/Core/Intersection/PLatLongEnvSphere.h"
#include "Engine/Core/Emitter/LatLongEnvEmitter.h"
#include "Engine/Actor/Dome/AImageDome.h"
#include "Engine/Actor/Dome/APreethamDome.h"
#include "Engine/Core/Texture/Function/unary_texture_operators.h"
#include "Engine/World/Foundation/CookOrder.h"
#include "Engine/World/Foundation/PreCookReport.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/Core/Intersection/TMetaInjectionPrimitive.h"

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

	auto localToWorld = ctx.getResources().makeTransform<StaticRigidTransform>(
		StaticRigidTransform::makeForward(sanifiedLocalToWorld));
	auto worldToLocal = ctx.getResources().makeTransform<StaticRigidTransform>(
		StaticRigidTransform::makeInverse(sanifiedLocalToWorld));

	report.setBaseTransforms(localToWorld, worldToLocal);

	return report;
}

TransientVisualElement ADome::cook(const CookingContext& ctx, const PreCookReport& report) const
{
	// Must match what was used in `preCook()`
	auto localToWorld = static_cast<const StaticRigidTransform*>(report.getBaseLocalToWorld());
	auto worldToLocal = static_cast<const StaticRigidTransform*>(report.getBaseWorldToLocal());

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

	PrimitiveMetadata* metadata = ctx.getResources().makeMetadata();

	// A dome should not have any visible inter-reflections, ideally
	auto material = std::make_shared<IdealSubstance>();
	material->setSubstance(EIdealSubstance::Absorber);
	metadata->surface().setOptics(material->createCooked(ctx)->surfaceOptics);
	
	if(material->getOverlapPriority() > 0)
	{
		// TODO: volume optics
	}

	auto* domePrimitive = ctx.getResources().copyIntersectable(TMetaInjectionPrimitive(
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

	const SurfaceEmitter* domeEmitter = nullptr;
	if(!radianceFunctionInfo.isAnalytical)
	{
		domeEmitter = ctx.getResources().makeEmitter<LatLongEnvEmitter>(
			domePrimitive,
			radianceFunction,
			radianceFunctionInfo.resolution);
	}
	else
	{
		// FIXME: proper resolution for analytical emitter
		domeEmitter = ctx.getResources().makeEmitter<LatLongEnvEmitter>(
			domePrimitive,
			radianceFunction,
			math::Vector2S(512, 256));
	}

	metadata->surface().setEmitter(domeEmitter);
	
	// Store cooked data

	ctx.getResources().getNamed()->setBackgroundPrimitive(domePrimitive);

	TransientVisualElement result;
	result.surfaceEmitters.push_back(domeEmitter);
	return result;
}

CookOrder ADome::getCookOrder() const
{
	return CookOrder(ECookPriority::Low, ECookLevel::Last);
}

}// end namespace ph
