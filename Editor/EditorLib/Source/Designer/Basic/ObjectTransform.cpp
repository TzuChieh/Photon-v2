#include "Designer/Basic/ObjectTransform.h"

#include <Math/Transform/StaticAffineTransform.h>
#include <Math/Transform/StaticRigidTransform.h>

namespace ph::editor
{

ObjectTransform& ObjectTransform::setPosition(const real x, const real y, const real z)
{
	return setPosition({x, y, z});
}

ObjectTransform& ObjectTransform::setPosition(const math::Vector3R& position)
{
	m_position = position;

	return *this;
}

ObjectTransform& ObjectTransform::setRotation(const math::Vector3R& axis, const real degrees)
{
	const auto normalizedAxis = axis.normalize();
	return setRotation(math::QuaternionR(normalizedAxis, math::to_radians(degrees)));
}

ObjectTransform& ObjectTransform::setRotation(const math::QuaternionR& rotation)
{
	m_rotation = rotation;

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

math::Vector3R ObjectTransform::getPosition() const
{
	return m_position;
}

math::QuaternionR ObjectTransform::getRotation() const
{
	return m_rotation;
}

math::Vector3R ObjectTransform::getScale() const
{
	return m_scale;
}

math::TDecomposedTransform<real> ObjectTransform::getDecomposed() const
{
	return math::TDecomposedTransform<real>(
		m_position,
		m_rotation,
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
	m_position = decomposed.getPosition();
	m_rotation = decomposed.getRotation();
	m_scale = decomposed.getScale();

	return *this;
}

}// end namespace ph::editor
