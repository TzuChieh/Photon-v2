#pragma once

#include "Common/primitive_type.h"
#include "Common/config.h"
#include "Utility/TFixedSizeStack.h"

#include <limits>

namespace ph
{

class Intersectable;
class IntersectionDetail;
class Ray;

class IntersectionProbe final
{
public:
	inline IntersectionProbe() : 
		m_hitStack(),
		m_hitRayT(std::numeric_limits<real>::infinity())
	{

	}

	// A convenient method for acquiring details on intersection.
	void calcIntersectionDetail(const Ray& ray, IntersectionDetail* out_detail);

	inline void pushIntermediateHit(const Intersectable* hitTarget)
	{
		m_hitStack.push(hitTarget);
	}

	inline void pushBaseHit(const Intersectable* hitTarget,
	                        const real hitRayT)
	{
		m_hitStack.push(hitTarget);
		m_hitRayT = hitRayT;
	}

	inline void popIntermediateHit()
	{
		m_hitStack.pop();
	}

	inline const Intersectable* getCurrentHit() const
	{
		return m_hitStack.get();
	}

	inline real getHitRayT() const
	{
		return m_hitRayT;
	}

	inline void clear()
	{
		m_hitStack.clear();
		m_hitRayT = std::numeric_limits<real>::infinity();
	}

private:
	typedef TFixedSizeStack<const Intersectable*, PH_INTERSECTION_PROBE_DEPTH> Stack;

	Stack m_hitStack;
	real  m_hitRayT;
};

}// end namespace ph