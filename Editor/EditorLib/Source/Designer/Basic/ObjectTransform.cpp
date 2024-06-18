#include "Designer/Basic/ObjectTransform.h"

#include <Math/Transform/StaticAffineTransform.h>
#include <Math/Transform/StaticRigidTransform.h>

namespace ph::editor
{

ObjectTransform& ObjectTransform::setPos(const real x, const real y, const real z)
{
	return setPos({x, y, z});
}

ObjectTransform& ObjectTransform::setPos(const math::Vector3R& pos)
{
	m_pos = pos;

	return *this;
}

ObjectTransform& ObjectTransform::setRot(const math::Vector3R& axis, const real degrees)
{
	const auto normalizedAxis = axis.normalize();
	return setRot(math::QuaternionR(normalizedAxis, math::to_radians(degrees)));
}

ObjectTransform& ObjectTransform::setRot(const math::QuaternionR& rot)
{
	m_rot = rot;

	return *this;
}

ObjectTransform& ObjectTransform::setScale(const real uniformScale)
{
	return setScale(uniformScale, uniformScale, uniformScale);
}

ObjectTransform& ObjectTransform::setScale(const real x, const real y, const real z)
{
	return setScale({x, y, z});
}

ObjectTransform& ObjectTransform::setScale(const math::Vector3R& scale)
{
	m_scale = scale;

	return *this;
}

ObjectTransform& ObjectTransform::translate(const real amountX, const real amountY, const real amountZ)
{
	return translate({amountX, amountY, amountZ});
}

ObjectTransform& ObjectTransform::translate(const math::Vector3R& amount)
{
	return set(getDecomposed().translate(amount));
}

ObjectTransform& ObjectTransform::rotate(const math::Vector3R& axis, const real additionalDegrees)
{
	const auto normalizedAxis = axis.normalize();
	return rotate(math::QuaternionR(normalizedAxis, math::to_radians(additionalDegrees)));
}

ObjectTransform& ObjectTransform::rotate(const math::QuaternionR& additionalRotation)
{
	return set(getDecomposed().rotate(additionalRotation));
}

ObjectTransform& ObjectTransform::scale(real uniformAmount)
{
	return scale(uniformAmount, uniformAmount, uniformAmount);
}

ObjectTransform& ObjectTransform::scale(const real amountX, const real amountY, const real amountZ)
{
	return scale({amountX, amountY, amountZ});
}

ObjectTransform& ObjectTransform::scale(const math::Vector3R& amount)
{
	return set(getDecomposed().scale(amount));
}

math::Vector3R ObjectTransform::getPos() const
{
	return m_pos;
}

math::QuaternionR ObjectTransform::getRot() const
{
	return m_rot;
}

math::Vector3R ObjectTransform::getScale() const
{
	return m_scale;
}

math::TDecomposedTransform<real> ObjectTransform::getDecomposed() const
{
	return math::TDecomposedTransform<real>(
		m_pos,
		m_rot,
		m_scale);
}

math::StaticAffineTransform ObjectTransform::getForwardStaticAffine() const
{
	return math::StaticAffineTransform::makeForward(getDecomposed());
}

math::StaticAffineTransform ObjectTransform::getInverseStaticAffine() const
{
	return math::StaticAffineTransform::makeInverse(getDecomposed());
}

math::StaticRigidTransform ObjectTransform::getForwardStaticRigid() const
{
	return math::StaticRigidTransform::makeForward(getDecomposed());
}

math::StaticRigidTransform ObjectTransform::getInverseStaticRigid() const
{
	return math::StaticRigidTransform::makeInverse(getDecomposed());
}

ObjectTransform& ObjectTransform::set(const math::TDecomposedTransform<real>& decomposed)
{
	m_pos = decomposed.getPos();
	m_rot = decomposed.getRot();
	m_scale = decomposed.getScale();

	return *this;
}

}// end namespace ph::editor
