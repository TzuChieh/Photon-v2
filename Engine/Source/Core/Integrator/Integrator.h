#pragma once

#include "Core/Integrator/Utility/SenseEvent.h"
#include "Core/Sample.h"
#include "Camera/Camera.h"

#include <vector>

namespace ph
{

class Scene;
class Ray;
class Camera;
class InputPacket;

class Integrator
{
public:
	Integrator();
	Integrator(const InputPacket& packet);
	virtual ~Integrator() = 0;

	virtual void update(const Scene& scene) = 0;
	virtual void radianceAlongRay(const Sample& sample, const Scene& scene, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const = 0;
};

}// end namespace ph