#include "Engine/World/Foundation/CookedNamedResource.h"
#include "Engine/Core/Intersection/Primitive.h"

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
	if(primitive && primitive->numMetadataSlots() > 1)
	{
		PH_LOG(CookedNamedResource, Warning,
			"{} metadata in background primitive, will use metadata slot 0 only",
			primitive->numMetadataSlots());
	}

	// Overwriting existing background primitive is not what the user want (most of the time).
	// Log this event so it is easier to track.
	if(m_backgroundPrimitive != nullptr)
	{
		PH_LOG(CookedNamedResource, Note,
			"overwriting existing background primitive");
	}

	m_backgroundPrimitive = primitive;
}

}// end namespace ph
