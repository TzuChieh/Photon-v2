#pragma once

#include "Math/TVector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix4f.h"
#include "Math/Transform.h"

namespace ph
{

class TransformInfo final
{
public:
	TransformInfo();

	Transform genTransform(const Transform& parentTransform) const;
	Transform genInverseTransform(const Transform& parentInverseTransform) const;

	inline const Vector3R& getPosition() const
	{
		return m_position;
	}

	inline const Quaternion& getRotation() const
	{
		return m_rotation;
	}

	inline const Vector3R& getScale() const
	{
		return m_scale;
	}

	inline void setPosition(const Vector3R& position)
	{
		m_position = position;
	}

	inline void setRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}

	inline void setScale(const Vector3R& scale)
	{
		m_scale = scale;
	}

private:
	Vector3R   m_position;
	Quaternion m_rotation;
	Vector3R   m_scale;
};

}// end namespace ph