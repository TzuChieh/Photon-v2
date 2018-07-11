#pragma once

#include "ph_core.h"
#include "Core/Renderer/EAttribute.h"
#include "Common/assertion.h"

#include <iostream>

namespace ph
{

class ApiHelper final
{
public:
	static EAttribute toCppAttribute(PH_EATTRIBUTE attribute);
};

// In-header Implementations:

inline EAttribute ApiHelper::toCppAttribute(const PH_EATTRIBUTE attribute)
{
	switch(attribute)
	{
	case PH_EATTRIBUTE::LIGHT_ENERGY:
		return EAttribute::LIGHT_ENERGY;
		break;

	case PH_EATTRIBUTE::NORMAL:
		return EAttribute::NORMAL;
		break;

	case PH_EATTRIBUTE::DEPTH:
		return EAttribute::DEPTH;
		break;

	default:
		std::cerr << "warning: cannot convert to cpp attribute" << std::endl;
		return EAttribute::LIGHT_ENERGY;
		break;
	}

	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph