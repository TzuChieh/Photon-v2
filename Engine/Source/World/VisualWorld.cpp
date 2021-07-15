#include "World/VisualWorld.h"
#include "Common/primitive_type.h"
#include "Core/Ray.h"
#include "Actor/Actor.h"
#include "Actor/CookedUnit.h"
#include "Actor/ActorCookingContext.h"
#include "Core/Intersectable/BruteForceIntersector.h"
#include "Core/Intersectable/Kdtree/KdtreeIntersector.h"
#include "Core/Emitter/Sampler/ESUniformRandom.h"
#include "Core/Intersectable/Bvh/ClassicBvhIntersector.h"
#include "World/VisualWorldInfo.h"
#include "Core/Intersectable/Intersector/TIndexedKdtreeIntersector.h"
#include "Core/Emitter/Sampler/ESPowerFavoring.h"
#include "Actor/APhantomModel.h"

#include <limits>
#include <iostream>
#include <algorithm>
#include <iterator>

namespace ph
{

const Logger VisualWorld::logger(LogSender("Visual World"));

VisualWorld::VisualWorld() :
	m_intersector(),
	//m_emitterSampler(std::make_shared<ESUniformRandom>()),
	m_emitterSampler(std::make_shared<ESPowerFavoring>()),
	m_scene(),
	m_receiverPos(0),
	m_backgroundPrimitive(nullptr)
{
	setCookSettings(CookSettings());
}

VisualWorld::VisualWorld(VisualWorld&& other) :
	m_actors             (std::move(other.m_actors)), 
	m_cookedActorStorage (std::move(other.m_cookedActorStorage)), 
	m_intersector        (std::move(other.m_intersector)), 
	m_emitterSampler     (std::move(other.m_emitterSampler)),
	m_scene              (std::move(other.m_scene)),
	m_receiverPos        (std::move(other.m_receiverPos)),
	m_backgroundPrimitive(std::move(other.m_backgroundPrimitive))
{}

void VisualWorld::addActor(std::shared_ptr<Actor> actor)
{
	// TODO: allow duplicated actors?

	if(actor != nullptr)
	{
		m_actors.push_back(actor);
	}
	else
	{
		std::cerr << "warning: at VisualWorld::addActor(), input is null" << std::endl;
	}
}

void VisualWorld::cook()
{
	logger.log(ELogLevel::NOTE_MED, "cooking visual world...");

	// TODO: clear cooked data

	CookingContext cookingContext;

	VisualWorldInfo visualWorldInfo;
	cookingContext.setVisualWorldInfo(&visualWorldInfo);

	// TODO: should union with receiver's bound instead
	visualWorldInfo.setRootActorsBound(math::AABB3D(m_receiverPos));
	visualWorldInfo.setLeafActorsBound(math::AABB3D(m_receiverPos));

	// Cook actors level by level (from lowest to highest)

	std::size_t numCookedActors = 0;
	while(numCookedActors < m_actors.size())
	{
		auto actorCookBegin = m_actors.begin() + numCookedActors;

		// Sort raw actors based on cook order
		std::sort(actorCookBegin, m_actors.end(),
			[](const std::shared_ptr<Actor>& a, const std::shared_ptr<Actor>& b)
			{
				return a->getCookOrder() < b->getCookOrder();
			});

		const CookLevel currentActorLevel = (*actorCookBegin)->getCookOrder().level;
		logger.log("cooking actor level: " + std::to_string(currentActorLevel));

		// Find the transition point from current level to next level
		auto actorCookEnd = std::upper_bound(actorCookBegin, m_actors.end(), currentActorLevel,
			[](const CookLevel a, const std::shared_ptr<Actor>& b)
			{
				return a < b->getCookOrder().level;
			});

		cookActors(&m_actors[numCookedActors], actorCookEnd - actorCookBegin, cookingContext);

		// Prepare for next cooking iteration

		// FIXME: calc bounds from newly cooked actors and union
		math::AABB3D bound = calcIntersectableBound(m_cookedActorStorage);
		// TODO: should union with receiver's bound instead
		bound.unionWith(m_receiverPos);

		logger.log("current iteration actor bound: " + bound.toString());

		if(currentActorLevel == static_cast<CookLevel>(ECookLevel::FIRST))
		{
			logger.log("root actors bound calculated to be: " + bound.toString());

			visualWorldInfo.setRootActorsBound(bound);
		}

		visualWorldInfo.setLeafActorsBound(bound);

		// Add newly created actors
		auto childActors = cookingContext.claimChildActors();
		m_actors.insert(m_actors.end(), std::make_move_iterator(childActors.begin()), std::make_move_iterator(childActors.end()));

		numCookedActors += actorCookEnd - actorCookBegin;

		logger.log("# cooked actors: " + std::to_string(numCookedActors));
	}// end while more raw actors

	for(auto& phantom : cookingContext.m_phantoms)
	{
		phantom.second.claimCookedData(m_phantomStorage);
		phantom.second.claimCookedBackend(m_phantomStorage);
	}

	m_backgroundPrimitive = cookingContext.claimBackgroundPrimitive();

	logger.log(ELogLevel::NOTE_MED, 
	           "visual world discretized into " + 
	           std::to_string(m_cookedActorStorage.numIntersectables()) + 
	           " intersectables");

	logger.log(ELogLevel::NOTE_MED, 
	           "number of emitters: " + 
	           std::to_string(m_cookedActorStorage.numEmitters()));

	logger.log(ELogLevel::NOTE_MED, "updating accelerator...");
	createTopLevelAccelerator();
	m_intersector->update(m_cookedActorStorage);

	logger.log(ELogLevel::NOTE_MED, "updating light sampler...");
	m_emitterSampler->update(m_cookedActorStorage);

	m_scene = std::make_shared<Scene>(m_intersector.get(), m_emitterSampler.get());
	m_scene->setBackgroundPrimitive(m_backgroundPrimitive.get());
}

void VisualWorld::cookActors(
	std::shared_ptr<Actor>* const actors,
	const std::size_t             numActors,
	CookingContext&               cookingContext)
{
	PH_ASSERT(actors);

	for(std::size_t i = 0; i < numActors; ++i)
	{
		CookedUnit cookedUnit = actors[i]->cook(cookingContext);
		cookedUnit.claimCookedData(m_cookedActorStorage);
		cookedUnit.claimCookedBackend(m_cookedBackendStorage);// TODO: make backend phantoms
	}
}

void VisualWorld::createTopLevelAccelerator()
{
	const EAccelerator type = m_cookSettings.getTopLevelAccelerator();

	std::string name;
	switch(type)
	{
	case EAccelerator::BRUTE_FORCE:
		m_intersector = std::make_shared<BruteForceIntersector>();
		name = "Brute-Force";
		break;

	case EAccelerator::BVH:
		m_intersector = std::make_shared<ClassicBvhIntersector>();
		name = "BVH";
		break;

	case EAccelerator::KDTREE:
		m_intersector = std::make_shared<KdtreeIntersector>();
		name = "kD-Tree";
		break;

	// FIXME: need to ensure sufficient max value
	case EAccelerator::INDEXED_KDTREE:
		m_intersector = std::make_shared<TIndexedKdtreeIntersector<int>>();
		name = "Indexed kD-Tree";
		break;

	default:
		m_intersector = std::make_shared<ClassicBvhIntersector>();
		name = "BVH";
		break;
	}

	logger.log("top level accelerator type: " + name);
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

}// end namespace ph
