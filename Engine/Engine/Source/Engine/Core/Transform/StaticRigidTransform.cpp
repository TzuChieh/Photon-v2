#include "Engine/Core/Transform/StaticRigidTransform.h"

namespace ph
{

const StaticRigidTransform& StaticRigidTransform::makeIdentity()
{
	static const StaticRigidTransform identityTransform(StaticAffineTransform::makeIdentity());
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
