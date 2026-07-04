#include "Engine/Core/Transform/StaticRigidTransform.h"

namespace ph
{

const StaticRigidTransform& StaticRigidTransform::IDENTITY()
{
	static const StaticRigidTransform identityTransform(StaticAffineTransform::IDENTITY());
	return identityTransform;
}

StaticRigidTransform::StaticRigidTransform() : 
	m_staticTransform()
{}

StaticRigidTransform::StaticRigidTransform(const StaticAffineTransform& transform) :
	m_staticTransform(transform)
{
	// TODO: assert on scale-free
}

}// end namespace ph
