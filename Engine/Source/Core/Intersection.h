#pragma once

#include "Math/Vector3f.h"

namespace ph
{

class Intersection final
{
public:
	inline const Vector3f& getHitPosition() const
	{
		return m_hitPosition;
	}

	inline const Vector3f& getHitNormal() const
	{
		return m_hitNormal;
	}

	inline void setHitPosition(const Vector3f& hitPosition)
	{
		m_hitPosition.set(hitPosition);
	}

	inline void setHitNormal(const Vector3f& hitNormal)
	{
		m_hitNormal.set(hitNormal);
	}

private:
	Vector3f m_hitPosition;
	Vector3f m_hitNormal;
};

}// end namespace ph