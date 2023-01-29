#include "World/VisualWorld.h"
#include "Common/primitive_type.h"
#include "DataIO/SDL/SceneDescription.h"
#include "World/Foundation/CookedUnit.h"
#include "World/Foundation/CookingContext.h"
#include "EngineEnv/CoreCookingContext.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "Actor/Actor.h"
#include "Core/Intersectable/BruteForceIntersector.h"
#include "Core/Intersectable/Kdtree/KdtreeIntersector.h"
#include "Core/Emitter/Sampler/ESUniformRandom.h"
#include "Core/Intersectable/Bvh/ClassicBvhIntersector.h"
#include "Core/Intersectable/Intersector/TIndexedKdtreeIntersector.h"
#include "Core/Emitter/Sampler/ESPowerFavoring.h"
#include "Actor/APhantomModel.h"
#include "Common/logging.h"
#include "Common/stats.h"
#include "World/Foundation/CookOrder.h"
#include "World/Foundation/PreCookReport.h"

#include <limits>
#include <iostream>
#include <algorithm>
#include <iterator>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(VisualWorld, World);
PH_DEFINE_INTERNAL_TIMER_STAT(CookActorLevels, VisualWorld);
PH_DEFINE_INTERNAL_TIMER_STAT(UpdateAccelerators, VisualWorld);
PH_DEFINE_INTERNAL_TIMER_STAT(UpdateLightSamplers, VisualWorld);

VisualWorld::VisualWorld()
	: m_cookedResources(nullptr)
	, m_intersector()
	//m_emitterSampler(std::make_shared<ESUniformRandom>()),
	, m_emitterSampler(std::make_unique<ESPowerFavoring>())
	, m_scene()
	, m_receiverPos(0)
	, m_backgroundPrimitive(nullptr)
	, m_rootActorsBound(math::Vector3R(0))
	, m_leafActorsBound(math::Vector3R(0))
{}

//void VisualWorld::addActor(std::shared_ptr<Actor> actor)
//{
//	// TODO: allow duplicated actors?
//
//	if(actor != nullptr)
//	{
//		m_actors.push_back(actor);
//	}
//	else
//	{
//		std::cerr << "warning: at VisualWorld::addActor(), input is null" << std::endl;
//	}
//}

void VisualWorld::cook(const SceneDescription& rawScene, const CoreCookingContext& coreCtx)
{
	PH_LOG(VisualWorld, "cooking visual world");

	// Create the storage for cooked resources, and potentially free all previous resources
	m_cookedResources = std::make_unique<CookedResourceCollection>();

	std::vector<std::shared_ptr<Actor>> actors = rawScene.getResources<Actor>();

	// TODO: clear cooked data

	CookingContext ctx(this);

	// TODO: should set to be receiver's bounds instead
	m_rootActorsBound = math::AABB3D(m_receiverPos);
	m_leafActorsBound = math::AABB3D(m_receiverPos);

	// Cook actors level by level (from lowest to highest)

	std::size_t numCookedActors = 0;
	while(numCookedActors < actors.size())
	{
		PH_SCOPED_TIMER(CookActorLevels);

		auto actorCookBegin = actors.begin() + numCookedActors;

		// Sort raw actors based on cook order
		std::sort(actorCookBegin, actors.end(),
			[](const std::shared_ptr<Actor>& a, const std::shared_ptr<Actor>& b)
			{
				return a->getCookOrder() < b->getCookOrder();
			});

		const CookLevel currentActorLevel = (*actorCookBegin)->getCookOrder().level;
		PH_LOG(VisualWorld, "cooking actor level: {}", currentActorLevel);

		// Find the transition point from current level to next level
		auto actorCookEnd = std::upper_bound(actorCookBegin, actors.end(), currentActorLevel,
			[](const CookLevel a, const std::shared_ptr<Actor>& b)
			{
				return a < b->getCookOrder().level;
			});

		cookActors(&actors[numCookedActors], actorCookEnd - actorCookBegin, ctx);

		// Prepare for next cooking iteration

		// FIXME: calc bounds from newly cooked actors and union
		math::AABB3D bound = calcIntersectableBound(m_cookedActorStorage);
		// TODO: should union with receiver's bound instead
		bound.unionWith(m_receiverPos);

		PH_LOG(VisualWorld, "current iteration actor bound: {}", bound.toString());

		if(currentActorLevel == static_cast<CookLevel>(ECookLevel::FIRST))
		{
			PH_LOG(VisualWorld, "root actors bound calculated to be: {}", bound.toString());

			m_rootActorsBound = bound;
		}

		m_leafActorsBound = bound;

		// Add newly created actors
		auto childActors = ctx.claimChildActors();
		actors.insert(actors.end(), std::make_move_iterator(childActors.begin()), std::make_move_iterator(childActors.end()));

		numCookedActors += actorCookEnd - actorCookBegin;

		PH_LOG(VisualWorld, "# cooked actors: {}", numCookedActors);
	}// end while more raw actors

	for(auto& phantom : ctx.m_phantoms)
	{
		phantom.second.claimCookedData(m_phantomStorage);
		phantom.second.claimCookedBackend(m_phantomStorage);
	}

	m_backgroundPrimitive = ctx.claimBackgroundPrimitive();

	PH_LOG(VisualWorld, "visual world discretized into {} intersectables", 
		m_cookedActorStorage.numIntersectables());
	PH_LOG(VisualWorld, "number of emitters: {}", 
		m_cookedActorStorage.numEmitters());

	PH_LOG(VisualWorld, "updating accelerator...");
	{
		PH_SCOPED_TIMER(UpdateAccelerators);

		createTopLevelAccelerator(coreCtx.getTopLevelAcceleratorType());
		m_intersector->update(m_cookedActorStorage);
	}

	PH_LOG(VisualWorld, "updating light sampler...");
	{
		PH_SCOPED_TIMER(UpdateLightSamplers);

		m_emitterSampler->update(m_cookedActorStorage);
	}

	m_scene = std::make_unique<Scene>(m_intersector.get(), m_emitterSampler.get());
	m_scene->setBackgroundPrimitive(m_backgroundPrimitive.get());
}

void VisualWorld::cookActors(
	std::shared_ptr<Actor>* const actors,
	const std::size_t numActors,
	CookingContext& ctx)
{
	PH_ASSERT(actors);

	// TODO: parallel preCook() and postCook()

	for(std::size_t i = 0; i < numActors; ++i)
	{
		auto actor = actors[i];

		try
		{
			PreCookReport report = actor->preCook(ctx);
			CookedUnit cookedUnit = actor->cook(ctx, report);
			actor->postCook(ctx, cookedUnit);

			cookedUnit.claimCookedData(m_cookedActorStorage);
			cookedUnit.claimCookedBackend(m_cookedBackendStorage);// TODO: make backend phantoms
		}
		catch(const Exception& e)
		{
			PH_LOG_ERROR(VisualWorld,
				"error on cooking actor: {}", e.what());
		}
	}
}

void VisualWorld::createTopLevelAccelerator(const EAccelerator acceleratorType)
{
	std::string name;
	switch(acceleratorType)
	{
	case EAccelerator::BruteForce:
		m_intersector = std::make_unique<BruteForceIntersector>();
		name = "Brute-Force";
		break;

	case EAccelerator::BVH:
		m_intersector = std::make_unique<ClassicBvhIntersector>();
		name = "BVH";
		break;

	case EAccelerator::Kdtree:
		m_intersector = std::make_unique<KdtreeIntersector>();
		name = "kD-Tree";
		break;

	// FIXME: need to ensure sufficient max value
	case EAccelerator::IndexedKdtree:
		m_intersector = std::make_unique<TIndexedKdtreeIntersector<int>>();
		name = "Indexed kD-Tree";
		break;

	default:
		m_intersector = std::make_unique<ClassicBvhIntersector>();
		name = "BVH";
		break;
	}

	PH_LOG(VisualWorld, "top level accelerator type: {}", name);
}

math::AABB3D VisualWorld::calcIntersectableBound(const CookedDataStorage& storage)
{
	if(storage.numIntersectables() == 0)
	{
		return math::AABB3D();
	}

	math::AABB3D fullBound = storage.intersectables().begin()->get()->calcAABB();
	for(const auto& intersectable : storage.intersectables())
	{
		fullBound.unionWith(intersectable->calcAABB());
	}
	return fullBound;
}

CookedResourceCollection* VisualWorld::getCookedResources()
{
	return m_cookedResources.get();
}

math::AABB3D VisualWorld::getRootActorsBound() const
{
	return m_rootActorsBound;
}

math::AABB3D VisualWorld::getLeafActorsBound() const
{
	return m_leafActorsBound;
}

}// end namespace ph
