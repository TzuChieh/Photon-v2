#pragma once

#include "Common/assertion.h"

#include <memory>

namespace ph { class CoreCookedUnit; }

namespace ph
{

class CoreCookingContext final
{
public:
	explicit CoreCookingContext(CoreCookedUnit* cookedUnit);

	CoreCookedUnit& getCookedUnit() const;

private:
	CoreCookedUnit* m_cookedUnit;
};

// In-header Implementations:

inline CoreCookingContext::CoreCookingContext(
	CoreCookedUnit* const cookedUnit) : 

	m_cookedUnit(cookedUnit)
{
	PH_ASSERT(m_cookedUnit);
}

inline CoreCookedUnit& CoreCookingContext::getCookedUnit() const
{
	PH_ASSERT(m_cookedUnit);

	return *m_cookedUnit;
}

}// end namespace ph
