#pragma once

#include "Common/assertion.h"
#include "Math/TVector2.h"

#include <memory>

namespace ph
{

class CoreCookingContext final
{
public:
	CoreCookingContext();

	const math::TVector2<uint32>& getFrameSizePx() const;

	void setFrameSizePx(const math::TVector2<uint32>& frameSizePx);

private:
	math::TVector2<uint32> m_frameSizePx;
};

// In-header Implementations:

inline CoreCookingContext::CoreCookingContext() : 
	m_frameSizePx(0, 0)
{}

inline const math::TVector2<uint32>& CoreCookingContext::getFrameSizePx() const
{
	return m_frameSizePx;
}

inline void CoreCookingContext::setFrameSizePx(const math::TVector2<uint32>& frameSizePx)
{
	m_frameSizePx = frameSizePx;
}

}// end namespace ph
