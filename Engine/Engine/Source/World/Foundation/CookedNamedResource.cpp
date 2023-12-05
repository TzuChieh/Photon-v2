#include "World/Foundation/CookedNamedResource.h"

#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(CookedNamedResource, World);

CookedNamedResource::CookedNamedResource()
	: m_backgroundPrimitive(nullptr)
{}

const Primitive* CookedNamedResource::getBackgroundPrimitive() const
{
	return m_backgroundPrimitive;
}

void CookedNamedResource::setBackgroundPrimitive(const Primitive* const primitive)
{
	// Overwriting existing background primitive is not what the user want (most of the time).
	// Log this event so it is easier to track.
	if(m_backgroundPrimitive != nullptr)
	{
		PH_LOG(CookedNamedResource,
			"overwriting existing background primitive");
	}

	m_backgroundPrimitive = primitive;
}

}// end namespace ph
