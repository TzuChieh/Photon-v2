#include "World/World.h"
#include "Actor/Model/Geometry/Geometry.h"
#include "Core/Intersection.h"
#include "Common/primitive_type.h"
#include "Core/Ray.h"
#include "Actor/Model/Model.h"
#include "World/BruteForceIntersector.h"
#include "World/Kdtree/KdtreeIntersector.h"
#include "World/LightSampler/UniformRandomLightSampler.h"

#include <limits>
#include <iostream>

namespace ph
{

World::World() : 
	//m_intersector(std::make_unique<BruteForceIntersector>())
	m_intersector(std::make_unique<KdtreeIntersector>()), 
	m_lightSampler(std::make_unique<UniformRandomLightSampler>())
{

}

void World::addModel(const Model& model)
{
	m_models.push_back(model);
}

void World::addLight(const Light& light)
{
	m_lights.push_back(light);
}

void World::update(const float32 deltaS)
{
	std::cout << "updating world..." << std::endl;

	m_cookedModelStorage.clear();
	m_cookedLightStorage.clear();

	cookModels();
	cookLights();

	std::cout << "world discretized into " << m_cookedModelStorage.numPrimitives() << " primitives" << std::endl;
	std::cout << "processing..." << std::endl;

	m_intersector->update(m_cookedModelStorage);
	m_lightSampler->update(m_cookedLightStorage);
}

const Intersector& World::getIntersector() const
{
	return *m_intersector;
}

const LightSampler& World::getLightSampler() const
{
	return *m_lightSampler;
}

void World::cookModels()
{
	for(const auto& model : m_models)
	{
		model.cookData(&m_cookedModelStorage);
	}
}

void World::cookLights()
{
	for(const auto& light : m_lights)
	{
		light.cookData(&m_cookedModelStorage, &m_cookedLightStorage);
	}
}

}// end namespace ph