#pragma once

#include "Common/primitive_type.h"
#include "Core/Intersectable/Primitive.h"
#include "Actor/CookedActorStorage.h"
#include "Actor/Actor.h"
#include "World/Scene.h"
#include "Core/Intersectable/Intersector.h"
#include "World/LightSampler/LightSampler.h"

#include <vector>
#include <memory>

namespace ph
{

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
	CookedActorStorage m_cookedBackendStorage;

	std::unique_ptr<Intersector>  m_intersector;
	std::unique_ptr<LightSampler> m_lightSampler;
	Scene                         m_scene;
};

}// end namespace ph