#pragma once

#include "photonApi_Ph.h"

#include <ph_core.h>
#include <jni.h>

#include <iostream>

namespace ph
{

class JniHelper final
{
public:
	static PH_EATTRIBUTE toCAttribute(jint attribute);
};

// In-header Implementations:

inline PH_EATTRIBUTE JniHelper::toCAttribute(const jint attribute)
{
	switch(attribute)
	{
	case photonApi_Ph_ATTRIBUTE_LIGHT_ENERGY:
		return PH_EATTRIBUTE::LIGHT_ENERGY;
		break;

	case photonApi_Ph_ATTRIBUTE_NORMAL:
		return PH_EATTRIBUTE::NORMAL;
		break;

	case photonApi_Ph_ATTRIBUTE_DEPTH:
		return PH_EATTRIBUTE::DEPTH;
		break;

	default:
		std::cerr << "warning: cannot convert to c attribute" << std::endl;
		return PH_EATTRIBUTE(0);
		break;
	}
}

}// end namespace ph