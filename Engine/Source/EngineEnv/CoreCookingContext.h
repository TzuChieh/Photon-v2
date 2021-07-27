#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector2.h"
#include "EngineEnv/EAccelerator.h"

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
	EAccelerator getTopLevelAccelerator() const;

	void setFrameSizePx(const math::TVector2<uint32>& frameSizePx);
	void setNumWorkers(uint32 numWorkers);
	void setTopLevelAccelerator(EAccelerator accelerator);

private:
	math::TVector2<uint32> m_frameSizePx;
	uint32                 m_numWorkers;
	EAccelerator           m_topLevelAccelerator;
};

// In-header Implementations:

inline CoreCookingContext::CoreCookingContext() : 
	m_frameSizePx        (0, 0),
	m_numWorkers         (1),
	m_topLevelAccelerator(EAccelerator::UNSPECIFIED)
{}

inline const math::TVector2<uint32>& CoreCookingContext::getFrameSizePx() const
{
	return m_frameSizePx;
}

inline float64 CoreCookingContext::getAspectRatio() const
{
	PH_ASSERT_GT(getFrameSizePx().y, 0);

	return static_cast<float64>(getFrameSizePx().x) / static_cast<float64>(getFrameSizePx().y);
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

inline EAccelerator CoreCookingContext::getTopLevelAccelerator() const
{
	return m_topLevelAccelerator;
}

inline void CoreCookingContext::setTopLevelAccelerator(const EAccelerator accelerator)
{
	m_topLevelAccelerator = accelerator;
}

}// end namespace ph
