#pragma once

#include "Common/primitive_type.h"
#include "Core/Primitive/Primitive.h"
#include "Core/CookedActorStorage.h"
#include "Actor/Actor.h"
#include "World/Scene.h"
#include "World/Intersector/Intersector.h"
#include "World/LightSampler/LightSampler.h"

#include <vector>
#include <memory>

namespace ph
{

class Intersection;
class Ray;
class Actor;

class VisualWorld final
{
public:
	VisualWorld();
	VisualWorld(VisualWorld&& other);

	void cook();
	void addActor(std::shared_ptr<Actor> actor);

	const Scene& getScene() const;

	// forbid copying
	VisualWorld(const VisualWorld& other) = delete;
	VisualWorld& operator = (const VisualWorld& rhs) = delete;

private:
	std::vector<std::shared_ptr<Actor>> m_actors;
	CookedActorStorage m_cookedActorStorage;

	std::unique_ptr<Intersector>  m_intersector;
	std::unique_ptr<LightSampler> m_lightSampler;
	Scene                         m_scene;
};

}// end namespace ph