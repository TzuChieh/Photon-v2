#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/Quantity/Time.h"

namespace ph
{

class Ray final
{
public:
	class Differential;

public:
	Ray();
	Ray(const Vector3R& origin, const Vector3R& direction);
	Ray(const Vector3R& origin, const Vector3R& direction, real minT, real maxT);
	Ray(const Vector3R& origin, const Vector3R& direction, real minT, real maxT, const Time& time);

	// Points this ray in opposite direction and differential quantities are 
	// modified as appropriate; other attributes remain the same. This 
	// method essentially mirrored the ray with respect to its origin.
	inline Ray& reverse()
	{
		m_direction.mulLocal(-1);
		m_differential.m_dDdX.mulLocal(-1);
		m_differential.m_dDdY.mulLocal(-1);

		return *this;
	}

	inline void      setMinT(const real t)                  { m_minT = t;         }
	inline void      setMaxT(const real t)                  { m_maxT = t;         }
	inline void      setTime(const Time& time)              { m_time = time;      }
	inline void      setDirection(const Vector3R& dir)      { m_direction = dir;  }
	inline void      setOrigin(const Vector3R& pos)         { m_origin = pos;     }
	inline void      setDifferential(const Differential& d) { m_differential = d; }
    inline Vector3R& getOrigin()                            { return m_origin;    }
	inline Vector3R& getDirection()                         { return m_direction; }

	inline const Vector3R&     getOrigin()       const { return m_origin;       }
	inline const Vector3R&     getDirection()    const { return m_direction;    }
	inline const Time&         getTime()         const { return m_time;         }
	inline real                getMinT()         const { return m_minT;         }
	inline real                getMaxT()         const { return m_maxT;         }
	inline const Differential& getDifferential() const { return m_differential; }

public:
	class Differential final
	{
		friend class Ray;

	public:
		Differential();
		Differential(const Vector3R& dOdX, const Vector3R& dOdY, 
		             const Vector3R& dDdX, const Vector3R& dDdY);

		inline void setPartialOs(const Vector3R& dOdX, const Vector3R& dOdY)
		{
			m_dOdX = dOdX;
			m_dOdY = dOdY;
		}

		inline void setPartialDs(const Vector3R& dDdX, const Vector3R& dDdY)
		{
			m_dDdX = dDdX;
			m_dDdY = dDdY;
		}

		inline const Vector3R& getdOdX() const { return m_dOdX; }
		inline const Vector3R& getdOdY() const { return m_dOdY; }
		inline const Vector3R& getdDdX() const { return m_dDdX; }
		inline const Vector3R& getdDdY() const { return m_dDdY; }

	private:
		// Partial derivatives of ray origin and direction with respect to
		// raster x and y directions.
		Vector3R m_dOdX, m_dOdY;
		Vector3R m_dDdX, m_dDdY;
	};

private:
	Vector3R     m_origin;
	Vector3R     m_direction;
	real         m_minT;
	real         m_maxT;
	Time         m_time;
	Differential m_differential;
};

}// end namespace ph