#pragma once

#include "Common/primitive_type.h"
#include "Actor/Model/Model.h"
#include "Intersector.h"
#include "Core/Primitive/Primitive.h"
#include "Core/CookedModelStorage.h"
#include "World/LightSampler/LightSampler.h"
#include "Core/CookedLightStorage.h"
#include "Actor/Light/Light.h"

#include <vector>
#include <memory>

namespace ph
{

class Intersection;
class Ray;

class World final
{
public:
	World();

	void addModel(const Model& model);
	void addLight(const Light& light);
	void update(const float32 deltaS);

	const Intersector&  getIntersector() const;
	const LightSampler& getLightSampler() const;

private:
	std::vector<Model> m_models;
	std::vector<Light> m_lights;

	CookedModelStorage m_cookedModelStorage;
	CookedLightStorage m_cookedLightStorage;

	std::unique_ptr<Intersector> m_intersector;
	std::unique_ptr<LightSampler> m_lightSampler;

	void cookModels();
	void cookLights();
};

}// end namespace ph