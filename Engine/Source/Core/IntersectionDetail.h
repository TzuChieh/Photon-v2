#pragma once

#include "Math/TVector3.h"

namespace ph
{

class Primitive;

class IntersectionDetail final
{
public:
	IntersectionDetail();

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

	inline real getHitRayT() const
	{
		return m_hitRayT;
	}

	inline void set(const Primitive* primitive, 
	                const Vector3R&  hitPosition,
	                const Vector3R&  hitSmoothNormal,
	                const Vector3R&  hitGeoNormal,
	                const Vector3R&  hitUVW, 
	                const real       hitRayT)
	{
		m_hitPrimitive    = primitive;
		m_hitPosition     = hitPosition;
		m_hitSmoothNormal = hitSmoothNormal;
		m_hitGeoNormal    = hitGeoNormal;
		m_hitUVW          = hitUVW;
		m_hitRayT         = hitRayT;
	}

private:
	const Primitive* m_hitPrimitive;
	Vector3R         m_hitPosition;
	Vector3R         m_hitSmoothNormal;
	Vector3R         m_hitGeoNormal;
	Vector3R         m_hitUVW;
	real             m_hitRayT;
};

}// end namespace ph