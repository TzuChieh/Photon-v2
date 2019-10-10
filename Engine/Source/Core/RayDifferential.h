#pragma once

#include "Math/TVector3.h"

#include <limits>

namespace ph
{

class RayDifferential final
{
public:
	RayDifferential();
	RayDifferential(const math::Vector3R& dPdX, const math::Vector3R& dPdY,
	                const math::Vector3R& dDdX, const math::Vector3R& dDdY);

	// Propagate differential quantities to some location on a surface the ray
	// has intersected. Specifically, calculate ray differentials on location
	// <surfaceP> with surface normal <surfaceN>.
	void transferToSurface(const math::Vector3R& surfaceP, const math::Vector3R& surfaceN);

	// Modify differential quantities as if the ray is reversed in
	// direction.
	inline void reverse()
	{
		m_dDdX.mulLocal(-1);
		m_dDdY.mulLocal(-1);
	}

	inline void setPartialPs(const math::Vector3R& dPdX, const math::Vector3R& dPdY)
	{
		m_dPdX = dPdX;
		m_dPdY = dPdY;

		m_isPartialPsNonZero = isVectorNonZero(dPdX) || isVectorNonZero(dPdY);
	}

	inline void setPartialDs(const math::Vector3R& dDdX, const math::Vector3R& dDdY)
	{
		m_dDdX = dDdX;
		m_dDdY = dDdY;

		m_isPartialDsNonZero = isVectorNonZero(dDdX) || isVectorNonZero(dDdY);
	}

	inline const math::Vector3R& getdPdX() const { return m_dPdX; }
	inline const math::Vector3R& getdPdY() const { return m_dPdY; }
	inline const math::Vector3R& getdDdX() const { return m_dDdX; }
	inline const math::Vector3R& getdDdY() const { return m_dDdY; }

	inline bool isNonZero() const { return m_isPartialPsNonZero || m_isPartialDsNonZero; }

private:
	// A point P on a ray.
	math::Vector3R m_P;

	// Direction D of a ray.
	math::Vector3R m_D;

	// Partial derivatives for P & D with respect to raster coordinates x & y.
	math::Vector3R m_dPdX, m_dPdY;
	math::Vector3R m_dDdX, m_dDdY;

	bool m_isPartialPsNonZero;
	bool m_isPartialDsNonZero;

	static inline bool isVectorNonZero(const math::Vector3R& vec)
	{
		return vec.lengthSquared() > std::numeric_limits<real>::epsilon();
	}
};

}// end namespace ph
