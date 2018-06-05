#pragma once

#include "Core/Integrator/Utility/SenseEvent.h"
#include "Core/Sample.h"
#include "Core/Camera/Camera.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Renderer/RenderWork.h"

#include <vector>

namespace ph
{

class Scene;
class Ray;
class Camera;
class InputPacket;

class Integrator : public TCommandInterface<Integrator>
{
public:
	Integrator();
	virtual ~Integrator();

	virtual void integrate();

	virtual void update(const Scene& scene) = 0;
	virtual void radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const = 0;

// command interface
public:
	Integrator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph