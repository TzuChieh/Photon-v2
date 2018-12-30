#include "World/VisualWorld.h"
#include "Common/primitive_type.h"
#include "Core/Ray.h"
#include "Actor/Actor.h"
#include "Actor/CookedUnit.h"
#include "Actor/CookingContext.h"
#include "Core/Intersectable/BruteForceIntersector.h"
#include "Core/Intersectable/Kdtree/KdtreeIntersector.h"
#include "Core/Emitter/Sampler/ESUniformRandom.h"
#include "Core/Intersectable/Bvh/ClassicBvhIntersector.h"
#include "World/VisualWorldInfo.h"
#include "Core/Intersectable/IndexedKdtree/TIndexedKdtreeIntersector.h"
#include "Core/Emitter/Sampler/ESPowerFavoring.h"
#include "Actor/APhantomModel.h"

#include <limits>
#include <iostream>
#include <algorithm>

namespace ph
{

const Logger VisualWorld::logger(LogSender("Visual World"));

VisualWorld::VisualWorld() :

	m_intersector(),
	//m_emitterSampler(std::make_unique<ESUniformRandom>()),
	m_emitterSampler(std::make_unique<ESPowerFavoring>()),
	m_scene(),
	m_cameraPos(0),
	m_cookSettings(),

	m_backgroundEmitterPrimitive(nullptr)
{
	setCookSettings(std::make_shared<CookSettings>());
}

VisualWorld::VisualWorld(VisualWorld&& other) :
	m_actors            (std::move(other.m_actors)), 
	m_cookedActorStorage(std::move(other.m_cookedActorStorage)), 
	m_intersector       (std::move(other.m_intersector)), 
	m_emitterSampler    (std::move(other.m_emitterSampler)),
	m_scene             (std::move(other.m_scene)),
	m_cameraPos         (std::move(other.m_cameraPos)),
	m_cookSettings      (std::move(other.m_cookSettings)),

	m_backgroundEmitterPrimitive(std::move(other.m_backgroundEmitterPrimitive))
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

	// cook root actors
	cookActors(cookingContext);

	VisualWorldInfo visualWorldInfo;
	AABB3D bound = calcIntersectableBound(m_cookedActorStorage);

	// TODO: should union with camera's bound instead
	bound.unionWith(m_cameraPos);

	visualWorldInfo.setRootActorsBound(bound);
	cookingContext.setVisualWorldInfo(&visualWorldInfo);

	logger.log(ELogLevel::NOTE_MED, "root actors bound calculated to be: " + bound.toString());

	// cook child actors (breadth first)
	while(true)
	{
		auto childActors = cookingContext.claimChildActors();
		if(childActors.empty())
		{
			break;
		}

		for(const auto& actor : childActors)
		{
			CookedUnit cookedUnit = actor->cook(cookingContext);

			// HACK
			if(cookedUnit.isBackgroundEmitter())
			{
				m_backgroundEmitterPrimitive = cookedUnit.getBackgroundEmitterPrimitive();
			}

			cookedUnit.claimCookedData(m_cookedActorStorage);
			cookedUnit.claimCookedBackend(m_cookedBackendStorage);
		}
	}

	for(auto& phantom : cookingContext.m_phantoms)
	{
		phantom.second.claimCookedData(m_phantomStorage);
		phantom.second.claimCookedBackend(m_phantomStorage);
	}

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

	m_scene = Scene(m_intersector.get(), m_emitterSampler.get());

	// HACK
	if(m_backgroundEmitterPrimitive)
	{
		m_scene.setBackgroundEmitterPrimitive(m_backgroundEmitterPrimitive);
	}
}

void VisualWorld::cookActors(CookingContext& cookingContext)
{
	std::sort(m_actors.begin(), m_actors.end(), 
		[](const std::shared_ptr<Actor>& a, const std::shared_ptr<Actor>& b)
		{
			return a->getCookPriority() < b->getCookPriority();
		});

	for(const auto& actor : m_actors)
	{
		CookedUnit cookedUnit = actor->cook(cookingContext);

		// HACK
		if(cookedUnit.isBackgroundEmitter())
		{
			m_backgroundEmitterPrimitive = cookedUnit.getBackgroundEmitterPrimitive();
		}

		cookedUnit.claimCookedData(m_cookedActorStorage);
		cookedUnit.claimCookedBackend(m_cookedBackendStorage);
	}
}

void VisualWorld::createTopLevelAccelerator()
{
	PH_ASSERT(m_cookSettings);

	const EAccelerator type = m_cookSettings->getTopLevelAccelerator();

	std::string name;
	switch(type)
	{
	case EAccelerator::BRUTE_FORCE:
		m_intersector = std::make_unique<BruteForceIntersector>();
		name = "Brute-Force";
		break;

	case EAccelerator::BVH:
		m_intersector = std::make_unique<ClassicBvhIntersector>();
		name = "BVH";
		break;

	case EAccelerator::KDTREE:
		m_intersector = std::make_unique<KdtreeIntersector>();
		name = "kD-Tree";
		break;

	case EAccelerator::INDEXED_KDTREE:
		m_intersector = std::make_unique<TIndexedKdtreeIntersector<TIndexedKdtree<const Intersectable*, int>>>();
		name = "Indexed kD-Tree";
		break;

	default:
		m_intersector = std::make_unique<ClassicBvhIntersector>();
		name = "BVH";
		break;
	}

	logger.log("top level accelerator type: " + name);
}

const Scene& VisualWorld::getScene() const
{
	return m_scene;
}

AABB3D VisualWorld::calcIntersectableBound(const CookedDataStorage& storage)
{
	if(storage.numIntersectables() == 0)
	{
		return AABB3D();
	}

	AABB3D fullBound;
	storage.intersectables().begin()->get()->calcAABB(&fullBound);
	for(const auto& intersectable : storage.intersectables())
	{
		AABB3D bound;
		intersectable->calcAABB(&bound);
		fullBound.unionWith(bound);
	}
	return fullBound;
}

}// end namespace ph