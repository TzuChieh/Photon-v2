#pragma once

#include "Core/Integrator/Utility/SenseEvent.h"
#include "Core/Sample.h"
#include "Core/Camera/Camera.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/Statistics.h"
#include "Core/Integrator/Attribute/EAttribute.h"
#include "Core/Bound/TAABB2D.h"

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

	virtual void supportedAttributes() const = 0;
	virtual void setDomainPx(const TAABB2D<int64>& domain) = 0;
	virtual void setIntegrand() = 0;
	virtual void integrate() = 0;
	virtual void asyncGetDomainAttribute(EAttribute type, HdrRgbFrame& out_frame) = 0;

	Statistics::Record asyncGetStatistics() const;

	virtual void update(const Scene& scene) = 0;
	virtual void radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const = 0;

protected:
	void updateStatistics(const Statistics::Record& statistics);

private:
	Statistics m_statistics;

// command interface
public:
	Integrator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph