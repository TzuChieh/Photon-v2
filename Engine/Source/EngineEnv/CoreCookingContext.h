#pragma once

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
	EAccelerator getTopLevelAccelerator() const;

	void setFrameSizePx(const math::TVector2<uint32>& frameSizePx);
	void setTopLevelAccelerator(EAccelerator accelerator);

private:
	math::TVector2<uint32> m_frameSizePx;
	EAccelerator           m_topLevelAccelerator;
};

// In-header Implementations:

inline CoreCookingContext::CoreCookingContext() : 
	m_frameSizePx        (0, 0),
	m_topLevelAccelerator(EAccelerator::UNSPECIFIED)
{}

inline const math::TVector2<uint32>& CoreCookingContext::getFrameSizePx() const
{
	return m_frameSizePx;
}

inline void CoreCookingContext::setFrameSizePx(const math::TVector2<uint32>& frameSizePx)
{
	m_frameSizePx = frameSizePx;
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
