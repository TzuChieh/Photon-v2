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

#include <limits>
#include <iostream>

namespace ph
{

const Logger VisualWorld::logger(LogSender("Visual World"));

VisualWorld::VisualWorld() :
	//m_intersector(std::make_unique<BruteForceIntersector>()), 
	//m_intersector(std::make_unique<KdtreeIntersector>()), 
	m_intersector(std::make_unique<ClassicBvhIntersector>()), 
	m_lightSampler(std::make_unique<UniformRandomLightSampler>()), 
	m_scene()
{}

VisualWorld::VisualWorld(VisualWorld&& other) :
	m_actors            (std::move(other.m_actors)), 
	m_cookedActorStorage(std::move(other.m_cookedActorStorage)), 
	m_intersector       (std::move(other.m_intersector)), 
	m_lightSampler      (std::move(other.m_lightSampler)), 
	m_scene             (std::move(other.m_scene))
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

	CookingContext cookingContext;
	for(const auto& actor : m_actors)
	{
		CookedUnit cookedUnit = actor->cook(cookingContext);
		cookedUnit.claimCookedData(m_cookedActorStorage);
	}
	cookingContext.claimBackendData(m_cookedBackendStorage);

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
}

const Scene& VisualWorld::getScene() const
{
	return m_scene;
}

}// end namespace ph