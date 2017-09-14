#include "World/VisualWorld.h"
#include "Core/Intersection.h"
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

VisualWorld::VisualWorld() :
	//m_intersector(std::make_unique<BruteForceIntersector>()), 
	//m_intersector(std::make_unique<KdtreeIntersector>()), 
	m_intersector(std::make_unique<ClassicBvhIntersector>()), 
	m_lightSampler(std::make_unique<UniformRandomLightSampler>()), 
	m_scene()
{

}

VisualWorld::VisualWorld(VisualWorld&& other) :
	m_actors            (std::move(other.m_actors)), 
	m_cookedActorStorage(std::move(other.m_cookedActorStorage)), 
	m_intersector       (std::move(other.m_intersector)), 
	m_lightSampler      (std::move(other.m_lightSampler)), 
	m_scene             (std::move(other.m_scene))
{

}

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
	std::cout << "cooking visual world..." << std::endl;

	CookingContext cookingContext;
	for(const auto& actor : m_actors)
	{
		CookedUnit cookedUnit = actor->cook(cookingContext);
		m_cookedActorStorage.add(std::move(cookedUnit));
	}

	std::cout << "visual world discretized into " 
	          << m_cookedActorStorage.numIntersectables() 
	          << " intersectables" << std::endl;
	std::cout << "preprocessing..." << std::endl;

	m_intersector->update(m_cookedActorStorage);
	m_lightSampler->update(m_cookedActorStorage);

	m_scene = Scene(m_intersector.get(), m_lightSampler.get());

	// HACK
	CookedUnit cookedUnit = cookingContext.toCooked();
	m_cookedActorStorage.add(std::move(cookedUnit));
}

const Scene& VisualWorld::getScene() const
{
	return m_scene;
}

}// end namespace ph