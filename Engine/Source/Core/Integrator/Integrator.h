#pragma once

#include "Core/Integrator/Utility/SenseEvent.h"
#include "Core/Sample.h"
#include "Camera/Camera.h"

#include <vector>

namespace ph
{

class World;
class Ray;
class Vector3f;
class Camera;

class Integrator
{
public:
	virtual ~Integrator() = 0;

	virtual void update(const World& world) = 0;
	virtual void radianceAlongRay(const Sample& sample, const World& world, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const = 0;
};

}// end namespace ph