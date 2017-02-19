#pragma once

#include "Common/primitive_type.h"
#include "Actor/AModel.h"
#include "Core/Primitive/Primitive.h"
#include "Core/CookedActorStorage.h"
#include "Actor/ALight.h"
#include "Core/CoreActor.h"
#include "World/Scene.h"
#include "World/Intersector/Intersector.h"
#include "World/LightSampler/LightSampler.h"

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

	void addActor(std::unique_ptr<Actor> actor);
	void update(const real deltaS);

	const Scene& getScene() const;

private:
	std::vector<std::unique_ptr<Actor>> m_actors;
	CookedActorStorage m_cookedActorStorage;

	std::unique_ptr<Intersector> m_intersector;
	std::unique_ptr<LightSampler> m_lightSampler;
	Scene m_scene;

	void cookActors();
};

}// end namespace ph