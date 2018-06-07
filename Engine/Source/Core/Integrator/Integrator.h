#pragma once

#include "Core/Integrator/Utility/SenseEvent.h"
#include "Core/Sample.h"
#include "Core/Camera/Camera.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/Statistics.h"
#include "Core/Integrator/Attribute/EAttribute.h"
#include "Core/Bound/TAABB2D.h"
#include "Core/Integrator/Attribute/AttributeTags.h"

#include <vector>
#include <memory>

namespace ph
{

class Scene;
class Ray;
class Camera;
class InputPacket;
class HdrRgbFrame;

class Integrator : public TCommandInterface<Integrator>
{
public:
	Integrator();
	virtual ~Integrator() = 0;

	virtual AttributeTags supportedAttributes() const = 0;
	virtual void setDomainPx(const TAABB2D<int64>& domain) = 0;
	virtual void setIntegrand(const RenderWork& integrand) = 0;
	virtual void integrate(const AttributeTags& requestedAttributes) = 0;
	virtual void asyncGetDomainAttribute(EAttribute type, HdrRgbFrame& out_frame) = 0;

	// TODO: semantics of integrator copying
	virtual std::unique_ptr<Integrator> makeCopy() const = 0;

	Statistics::Record asyncGetStatistics() const;

protected:
	Integrator(const Integrator& other);

	void updateStatistics(const Statistics::Record& statistics);

	Integrator& operator = (const Integrator& rhs);

private:
	Statistics m_statistics;

	friend void swap(Integrator& first, Integrator& second);

// command interface
public:
	Integrator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph