#include "Actor/Basic/TransformInfo.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Math/Transform/StaticRigidTransform.h"

namespace ph
{

TransformInfo& TransformInfo::setPosition(const real x, const real y, const real z)
{
	return setPosition({x, y, z});
}

TransformInfo& TransformInfo::setPosition(const math::Vector3R& position)
{
	m_position = position;

	return *this;
}

TransformInfo& TransformInfo::setRotation(const math::Vector3R& axis, const real degrees)
{
	const auto normalizedAxis = axis.normalize();
	return setRotation(math::QuaternionR(normalizedAxis, math::to_radians(degrees)));
}

TransformInfo& TransformInfo::setRotation(const math::QuaternionR& rotation)
{
	m_rotation = rotation;

	return *this;
}

TransformInfo& TransformInfo::setScale(const real uniformScale)
{
	return setScale(uniformScale, uniformScale, uniformScale);
}

TransformInfo& TransformInfo::setScale(const real x, const real y, const real z)
{
	return setScale({x, y, z});
}

TransformInfo& TransformInfo::setScale(const math::Vector3R& scale)
{
	m_scale = scale;

	return *this;
}

TransformInfo& TransformInfo::translate(const real amountX, const real amountY, const real amountZ)
{
	return translate({amountX, amountY, amountZ});
}

TransformInfo& TransformInfo::translate(const math::Vector3R& amount)
{
	return set(getDecomposed().translate(amount));
}

TransformInfo& TransformInfo::rotate(const math::Vector3R& axis, const real additionalDegrees)
{
	const auto normalizedAxis = axis.normalize();
	return rotate(math::QuaternionR(normalizedAxis, math::to_radians(additionalDegrees)));
}

TransformInfo& TransformInfo::rotate(const math::QuaternionR& additionalRotation)
{
	return set(getDecomposed().rotate(additionalRotation));
}

TransformInfo& TransformInfo::scale(real uniformAmount)
{
	return scale(uniformAmount, uniformAmount, uniformAmount);
}

TransformInfo& TransformInfo::scale(const real amountX, const real amountY, const real amountZ)
{
	return scale({amountX, amountY, amountZ});
}

TransformInfo& TransformInfo::scale(const math::Vector3R& amount)
{
	return set(getDecomposed().scale(amount));
}

math::Vector3R TransformInfo::getPosition() const
{
	return m_position;
}

math::QuaternionR TransformInfo::getRotation() const
{
	return m_rotation;
}

math::Vector3R TransformInfo::getScale() const
{
	return m_scale;
}

math::TDecomposedTransform<real> TransformInfo::getDecomposed() const
{
	return math::TDecomposedTransform<real>(
		m_position,
		m_rotation,
		m_scale);
}

math::StaticAffineTransform TransformInfo::getForwardStaticAffine() const
{
	return math::StaticAffineTransform::makeForward(getDecomposed());
}

math::StaticAffineTransform TransformInfo::getInverseStaticAffine() const
{
	return math::StaticAffineTransform::makeInverse(getDecomposed());
}

math::StaticRigidTransform TransformInfo::getForwardStaticRigid() const
{
	return math::StaticRigidTransform::makeForward(getDecomposed());
}

math::StaticRigidTransform TransformInfo::getInverseStaticRigid() const
{
	return math::StaticRigidTransform::makeInverse(getDecomposed());
}

TransformInfo& TransformInfo::set(const math::TDecomposedTransform<real>& decomposed)
{
	m_position = decomposed.getPosition();
	m_rotation = decomposed.getRotation();
	m_scale = decomposed.getScale();

	return *this;
}

}// end namespace ph
