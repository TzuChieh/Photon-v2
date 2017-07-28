#pragma once

#include "Core/Integrator/Utility/SenseEvent.h"
#include "Core/Sample.h"
#include "Core/Camera/Camera.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Renderer/RenderData.h"

#include <vector>

namespace ph
{

class Scene;
class Ray;
class Camera;
class InputPacket;

class Integrator : public TCommandInterface<Integrator>, public ISdlResource
{
public:
	Integrator();
	virtual ~Integrator() = 0;

	virtual void update(const Scene& scene) = 0;
	virtual void radianceAlongRay(const Ray& ray, const RenderData& data, std::vector<SenseEvent>& out_senseEvents) const = 0;

// command interface
public:
	Integrator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<Integrator>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph