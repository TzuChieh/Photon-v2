#pragma once

#include "Math/TVector3.h"

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
		m_hitSmoothNormal.set(0, 0, -1);
		m_hitGeoNormal.set(0, 0, -1);
		m_hitUVW.set(0, 0, 0);
		m_hitPrimitive = nullptr;
	}

	inline const Vector3R& getHitPosition() const
	{
		return m_hitPosition;
	}

	inline const Vector3R& getHitSmoothNormal() const
	{
		return m_hitSmoothNormal;
	}

	inline const Vector3R& getHitGeoNormal() const
	{
		return m_hitGeoNormal;
	}

	inline const Vector3R& getHitUVW() const
	{
		return m_hitUVW;
	}

	inline const Primitive* getHitPrimitive() const
	{
		return m_hitPrimitive;
	}

	inline void setHitPosition(const Vector3R& hitPosition)
	{
		m_hitPosition.set(hitPosition);
	}

	inline void setHitSmoothNormal(const Vector3R& hitSmoothNormal)
	{
		m_hitSmoothNormal.set(hitSmoothNormal);
	}

	inline void setHitGeoNormal(const Vector3R& hitGeoNormal)
	{
		m_hitGeoNormal.set(hitGeoNormal);
	}

	inline void setHitUVW(const Vector3R& hitUVW)
	{
		m_hitUVW.set(hitUVW);
	}

	inline void setHitPrimitive(const Primitive* const hitPrimitive)
	{
		m_hitPrimitive = hitPrimitive;
	}

private:
	Vector3R m_hitPosition;
	Vector3R m_hitSmoothNormal;
	Vector3R m_hitGeoNormal;
	Vector3R m_hitUVW;
	const Primitive* m_hitPrimitive;
};

}// end namespace ph