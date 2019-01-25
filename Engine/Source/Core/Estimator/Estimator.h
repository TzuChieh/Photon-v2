#pragma once

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
class Estimation;

class Estimator : public TCommandInterface<Estimator>
{
public:
	Estimator();
	virtual ~Estimator() = default;

	virtual AttributeTags supportedAttributes() const = 0;

	virtual void update(const Scene& scene) = 0;

	virtual void estimate(
		const Ray&           ray, 
		const Integrand&     integrand, 
		const AttributeTags& requestedAttributes, 
		Estimation&          out_estimation) const = 0;

// command interface
public:
	Estimator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph