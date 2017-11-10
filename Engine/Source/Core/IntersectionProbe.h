#pragma once

#include "Common/primitive_type.h"
#include "Common/config.h"
#include "Utility/TFixedSizeStack.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"

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
		m_hitRayT(std::numeric_limits<real>::infinity()),
		m_realCache()
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

	inline void cacheReal3(const int32 headIndex, const Vector3R& real3)
	{
		PH_ASSERT(headIndex >= 0 && 
		          headIndex + 2 < PH_INTERSECTION_PROBE_REAL_CACHE_SIZE);

		m_realCache[headIndex + 0] = real3.x;
		m_realCache[headIndex + 1] = real3.y;
		m_realCache[headIndex + 2] = real3.z;
	}

	inline void getCachedReal3(const int32 headIndex, Vector3R* const out_real3) const
	{
		PH_ASSERT(headIndex >= 0 && 
		          headIndex + 2 < PH_INTERSECTION_PROBE_REAL_CACHE_SIZE);

		out_real3->x = m_realCache[headIndex + 0];
		out_real3->y = m_realCache[headIndex + 1];
		out_real3->z = m_realCache[headIndex + 2];
	}

private:
	typedef TFixedSizeStack<const Intersectable*, PH_INTERSECTION_PROBE_DEPTH> Stack;

	Stack m_hitStack;
	real  m_hitRayT;
	real  m_realCache[PH_INTERSECTION_PROBE_REAL_CACHE_SIZE];
};

}// end namespace ph