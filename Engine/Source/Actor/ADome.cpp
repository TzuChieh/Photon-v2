#include "Actor/ADome.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Material/IdealSubstance.h"
#include "Actor/ActorCookingContext.h"
#include "World/VisualWorldInfo.h"
#include "Math/Transform/StaticRigidTransform.h"
#include "Core/Intersectable/PLatLongEnvSphere.h"
#include "Core/Emitter/LatLongEnvEmitter.h"
#include "Actor/Dome/AImageDome.h"
#include "Actor/Dome/APreethamDome.h"
#include "Core/Texture/Function/unary_texture_operators.h"
#include "Common/logging.h"

#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(DomeActor, Actor);

ADome::ADome() : 

	PhysicalActor(),

	m_energyScale(1.0_r)
{}

CookedUnit ADome::cook(ActorCookingContext& ctx)
{
	// Ensure reasonable transformation for the dome
	math::TDecomposedTransform<real> sanifiedLocalToWorld = m_localToWorld;
	if(sanifiedLocalToWorld.hasScaleEffect())
	{
		PH_LOG(DomeActor,
			"Scale detected and is ignored; scaling on dome light should "
			"be avoided as it does not have any effect. If resizing the dome is "
			"desired, it should be done by changing its radius.");

		sanifiedLocalToWorld.setScale(1);
	}

	auto localToWorld = std::make_unique<math::StaticRigidTransform>(math::StaticRigidTransform::makeForward(sanifiedLocalToWorld));
	auto worldToLocal = std::make_unique<math::StaticRigidTransform>(math::StaticRigidTransform::makeInverse(sanifiedLocalToWorld));

	// Get the sphere radius that can encompass all actors
	real domeRadius = 1000.0_r;
	if(ctx.getVisualWorldInfo())
	{
		const auto worldBound = ctx.getVisualWorldInfo()->getLeafActorsBound();
		for(auto vertex : worldBound.getBoundVertices())
		{
			constexpr real ENLARGEMENT = 1.01_r;

			const auto vertexToCenter = (vertex - math::Vector3R(sanifiedLocalToWorld.getPosition()));
			const real ri             = vertexToCenter.length() * ENLARGEMENT;

			domeRadius = std::max(ri, domeRadius);
		}
	}
	else
	{
		PH_LOG_WARNING(DomeActor,
			"No visual world information available, cannot access actor bounds."
			"Using {} as dome radius.", 
			domeRadius);
	}

	auto metadata = std::make_unique<PrimitiveMetadata>();

	// A dome should not have any visible inter-reflections, ideally
	auto material = std::make_shared<IdealSubstance>();
	material->asAbsorber();
	material->genBehaviors(ctx, *metadata);

	auto domePrimitive = std::make_unique<PLatLongEnvSphere>(
		metadata.get(),
		domeRadius,
		localToWorld.get(),
		worldToLocal.get());
	
	auto radianceFunction = loadRadianceFunction(ctx);
	if(m_energyScale != 1.0_r)
	{
		auto scaledRadianceFunction = std::make_shared<TUnaryTextureOperator<math::Spectrum, math::Spectrum, texfunc::SpectrumMultiplyScalar>>(
			radianceFunction, static_cast<math::ColorValue>(m_energyScale));

		radianceFunction = scaledRadianceFunction;
	}

	// HACK:
	std::unique_ptr<Emitter> domeEmitter;
	if(!isAnalytical())
	{
		auto resolution = getResolution();

		domeEmitter = std::make_unique<LatLongEnvEmitter>(
			domePrimitive.get(),
			radianceFunction,
			resolution);
	}
	else
	{
		domeEmitter = std::make_unique<LatLongEnvEmitter>(
			domePrimitive.get(),
			radianceFunction,
			math::Vector2S(512, 256));
	}

	metadata->getSurface().setEmitter(domeEmitter.get());
	
	// Store cooked data

	CookedUnit cookedActor;
	cookedActor.setPrimitiveMetadata(std::move(metadata));
	cookedActor.addTransform(std::move(localToWorld));
	cookedActor.addTransform(std::move(worldToLocal));
	cookedActor.setEmitter(std::move(domeEmitter));

	ctx.setBackgroundPrimitive(std::move(domePrimitive));

	return cookedActor;
}

ADome& ADome::operator = (const ADome& rhs)
{
	PhysicalActor::operator = (rhs);

	return *this;
}

void swap(ADome& first, ADome& second)
{
	// Enable ADL
	using std::swap;

	swap(static_cast<PhysicalActor&>(first), static_cast<PhysicalActor&>(second));
}

}// end namespace ph
