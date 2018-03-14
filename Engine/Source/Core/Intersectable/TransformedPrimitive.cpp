#include "Core/Intersectable/TransformedPrimitive.h"
#include "Common/assertion.h"
#include "Math/Transform/RigidTransform.h"

namespace ph
{

TransformedPrimitive::TransformedPrimitive(
	const Primitive*      const primitive,
	const RigidTransform* const localToWorld,
	const RigidTransform* const worldToLocal) : 

	Primitive(primitive != nullptr ? primitive->getMetadata() : nullptr),

	m_primitive(primitive),
	m_intersectable(primitive, localToWorld, worldToLocal)
{
	PH_ASSERT(primitive    != nullptr);
	PH_ASSERT(localToWorld != nullptr);
	PH_ASSERT(worldToLocal != nullptr);
}

TransformedPrimitive::~TransformedPrimitive() = default;

}// end namespace ph