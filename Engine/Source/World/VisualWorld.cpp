#include "World/VisualWorld.h"
#include "Common/primitive_type.h"
#include "Core/Ray.h"
#include "Actor/Actor.h"
#include "Actor/CookedUnit.h"
#include "Actor/CookingContext.h"
#include "Core/Intersectable/BruteForceIntersector.h"
#include "Core/Intersectable/Kdtree/KdtreeIntersector.h"
#include "World/LightSampler/UniformRandomLightSampler.h"
#include "Core/Intersectable/Bvh/ClassicBvhIntersector.h"
#include "World/VisualWorldInfo.h"
#include "Core/Intersectable/IndexedKdtree/TIndexedKdtreeIntersector.h"

#include <limits>
#include <iostream>

namespace ph
{

const Logger VisualWorld::logger(LogSender("Visual World"));

VisualWorld::VisualWorld() :

	//m_intersector(std::make_unique<BruteForceIntersector>()), 
	//m_intersector(std::make_unique<KdtreeIntersector>()), 
	//m_intersector(std::make_unique<ClassicBvhIntersector>()), 
	m_intersector(std::make_unique<TIndexedKdtreeIntersector<TIndexedKdtree<const Intersectable*, int>>>()),

	m_lightSampler(std::make_unique<UniformRandomLightSampler>()), 
	m_scene(),
	m_cameraPos(0),

	m_backgroundEmitterPrimitive(nullptr)
{}

VisualWorld::VisualWorld(VisualWorld&& other) :
	m_actors            (std::move(other.m_actors)), 
	m_cookedActorStorage(std::move(other.m_cookedActorStorage)), 
	m_intersector       (std::move(other.m_intersector)), 
	m_lightSampler      (std::move(other.m_lightSampler)), 
	m_scene             (std::move(other.m_scene)),
	m_cameraPos         (std::move(other.m_cameraPos)),

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
	//
	cookActors(cookingContext);

	VisualWorldInfo visualWorldInfo;
	AABB3D bound = calcIntersectableBound(m_cookedActorStorage);

	// TODO: should union with camera's bound instead
	bound.unionWith(m_cameraPos);

	visualWorldInfo.setRootActorsBound(bound);
	cookingContext.setVisualWorldInfo(&visualWorldInfo);

	logger.log(ELogLevel::NOTE_MED, "root actors bound calculated to be: " + bound.toString());

	// cook child actors (breadth first)
	//
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

	logger.log(ELogLevel::NOTE_MED, 
	           "visual world discretized into " + 
	           std::to_string(m_cookedActorStorage.numIntersectables()) + 
	           " intersectables");

	logger.log(ELogLevel::NOTE_MED, 
	           "number of emitters: " + 
	           std::to_string(m_cookedActorStorage.numEmitters()));

	logger.log(ELogLevel::NOTE_MED, "updating intersector...");
	m_intersector->update(m_cookedActorStorage);

	logger.log(ELogLevel::NOTE_MED, "updating light sampler...");
	m_lightSampler->update(m_cookedActorStorage);

	m_scene = Scene(m_intersector.get(), m_lightSampler.get());

	// HACK
	if(m_backgroundEmitterPrimitive)
	{
		m_scene.setBackgroundEmitterPrimitive(m_backgroundEmitterPrimitive);
	}
}

void VisualWorld::cookActors(CookingContext& cookingContext)
{
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