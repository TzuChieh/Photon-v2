#pragma once

#include "Math/Vector3f.h"
#include "Math/Quaternion.h"
#include "Math/Matrix4f.h"
#include "Math/Transform.h"

namespace ph
{

class TransformInfo final
{
public:
	TransformInfo();

	Transform genTransform() const;
	Transform genInverseTransform() const;

	inline const Vector3f& getPosition() const
	{
		return m_position;
	}

	inline const Quaternion& getRotation() const
	{
		return m_rotation;
	}

	inline const Vector3f& getScale() const
	{
		return m_scale;
	}

	inline void setPosition(const Vector3f& position)
	{
		m_position = position;
	}

	inline void setRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}

	inline void setScale(const Vector3f& scale)
	{
		m_scale = scale;
	}

private:
	Vector3f   m_position;
	Quaternion m_rotation;
	Vector3f   m_scale;
};

}// end namespace ph