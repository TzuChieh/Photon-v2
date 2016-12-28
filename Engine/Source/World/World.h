#pragma once

#include "Common/primitive_type.h"
#include "Actor/AModel.h"
#include "Intersector.h"
#include "Core/Primitive/Primitive.h"
#include "Core/CookedActorStorage.h"
#include "World/LightSampler/LightSampler.h"
#include "Actor/ALight.h"
#include "Core/CoreActor.h"

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
	void update(const float32 deltaS);

	const Intersector&  getIntersector() const;
	const LightSampler& getLightSampler() const;

private:
	std::vector<std::unique_ptr<Actor>> m_actors;
	CookedActorStorage m_cookedActorStorage;

	std::unique_ptr<Intersector> m_intersector;
	std::unique_ptr<LightSampler> m_lightSampler;

	void cookActors();
};

}// end namespace ph