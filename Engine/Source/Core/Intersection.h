#pragma once

#include "Math/Vector3f.h"

namespace ph
{

class Primitive;

class Intersection final
{
public:
	Intersection();

	inline void clear()
	{
		m_hitPosition.set(0, 0, 0);
		m_hitNormal.set(0, 0, -1);
		m_hitPrimitive = nullptr;
	}

	inline const Vector3f& getHitPosition() const
	{
		return m_hitPosition;
	}

	inline const Vector3f& getHitNormal() const
	{
		return m_hitNormal;
	}

	inline const Primitive* getHitPrimitive() const
	{
		return m_hitPrimitive;
	}

	inline void setHitPosition(const Vector3f& hitPosition)
	{
		m_hitPosition.set(hitPosition);
	}

	inline void setHitNormal(const Vector3f& hitNormal)
	{
		m_hitNormal.set(hitNormal);
	}

	inline void setHitPrimitive(const Primitive* const hitPrimitive)
	{
		m_hitPrimitive = hitPrimitive;
	}

private:
	Vector3f m_hitPosition;
	Vector3f m_hitNormal;
	const Primitive* m_hitPrimitive;
};

}// end namespace ph