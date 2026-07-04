#pragma once

#include "Engine/Math/TVector2.h"
#include "Engine/Core/Quantity/TimeStep.h"
#include "Engine/EngineEnv/EAccelerator.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

class CoreCookingContext final
{
public:
	CoreCookingContext();

	const math::TVector2<uint32>& getFrameSizePx() const;
	float64 getAspectRatio() const;
	uint32 numWorkers() const;
	const TimeStep& getTimeStep() const;
	EAccelerator getTopLevelAcceleratorType() const;

	void setFrameSizePx(const math::TVector2<uint32>& frameSizePx);
	void setNumWorkers(uint32 numWorkers);
	void setTimeStep(TimeStep timeStep);
	void setTopLevelAcceleratorType(EAccelerator accelerator);

private:
	math::TVector2<uint32> m_frameSizePx;
	uint32                 m_numWorkers;
	TimeStep               m_timeStep;
	EAccelerator           m_topLevelAcceleratorType;
};

// In-header Implementations:

inline CoreCookingContext::CoreCookingContext() : 
	m_frameSizePx            (0, 0),
	m_numWorkers             (1),
	m_timeStep               (),
	m_topLevelAcceleratorType(EAccelerator::Unspecified)
{}

inline const math::TVector2<uint32>& CoreCookingContext::getFrameSizePx() const
{
	return m_frameSizePx;
}

inline float64 CoreCookingContext::getAspectRatio() const
{
	PH_ASSERT_GT(getFrameSizePx().y(), 0);

	return static_cast<float64>(getFrameSizePx().x()) / static_cast<float64>(getFrameSizePx().y());
}

inline void CoreCookingContext::setFrameSizePx(const math::TVector2<uint32>& frameSizePx)
{
	m_frameSizePx = frameSizePx;
}

inline uint32 CoreCookingContext::numWorkers() const
{
	return m_numWorkers;
}

inline void CoreCookingContext::setNumWorkers(const uint32 numWorkers)
{
	m_numWorkers = numWorkers;
}

inline const TimeStep& CoreCookingContext::getTimeStep() const
{
	return m_timeStep;
}

inline void CoreCookingContext::setTimeStep(TimeStep timeStep)
{
	m_timeStep = timeStep;
}

inline EAccelerator CoreCookingContext::getTopLevelAcceleratorType() const
{
	return m_topLevelAcceleratorType;
}

inline void CoreCookingContext::setTopLevelAcceleratorType(const EAccelerator accelerator)
{
	m_topLevelAcceleratorType = accelerator;
}

}// end namespace ph
