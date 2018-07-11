#pragma once

#include "Core/Estimator/Utility/SenseEvent.h"
#include "Core/Sample.h"
#include "Core/Camera/Camera.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Estimator/Integrand.h"
#include "Core/Renderer/AttributeTags.h"

#include <vector>

namespace ph
{

class Scene;
class Ray;
class Camera;
class InputPacket;

class Estimator : public TCommandInterface<Estimator>
{
public:
	Estimator();
	virtual ~Estimator() = 0;

	//virtual 
	virtual void update(const Scene& scene) = 0;
	virtual void radianceAlongRay(const Ray& ray, const Integrand& integrand, std::vector<SenseEvent>& out_senseEvents) const = 0;

// command interface
public:
	Estimator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph