#pragma once

#include "Math/TVector3.h"

#include <limits>

namespace ph
{

class RayDifferential final
{
	friend class Ray;

public:
	RayDifferential();
	RayDifferential(const Vector3R& dPdX, const Vector3R& dPdY,
	                const Vector3R& dDdX, const Vector3R& dDdY);

	inline void setPartialPs(const Vector3R& dPdX, const Vector3R& dPdY)
	{
		m_dPdX = dPdX;
		m_dPdY = dPdY;

		m_isPartialPsNonZero = isVectorNonZero(dPdX) || isVectorNonZero(dPdY);
	}

	inline void setPartialDs(const Vector3R& dDdX, const Vector3R& dDdY)
	{
		m_dDdX = dDdX;
		m_dDdY = dDdY;

		m_isPartialDsNonZero = isVectorNonZero(dDdX) || isVectorNonZero(dDdY);
	}

	inline const Vector3R& getdPdX() const { return m_dPdX; }
	inline const Vector3R& getdPdY() const { return m_dPdY; }
	inline const Vector3R& getdDdX() const { return m_dDdX; }
	inline const Vector3R& getdDdY() const { return m_dDdY; }

	inline bool isNonZero() const { return m_isPartialPsNonZero || m_isPartialDsNonZero; }

private:
	// Partial derivatives for a specific point along a ray (P = position, 
	// D = direction) with respect to raster coordinates x & y.
	Vector3R m_dPdX, m_dPdY;
	Vector3R m_dDdX, m_dDdY;

	bool m_isPartialPsNonZero;
	bool m_isPartialDsNonZero;

	static inline bool isVectorNonZero(const Vector3R& vec)
	{
		return vec.lengthSquared() > std::numeric_limits<real>::epsilon();
	}
};

}// end namespace ph