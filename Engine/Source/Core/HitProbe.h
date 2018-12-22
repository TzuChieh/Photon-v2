#pragma once

#include "Common/primitive_type.h"
#include "Common/config.h"
#include "Utility/TFixedSizeStack.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"

#include <limits>
#include <array>
#include <cstdint>

namespace ph
{

class Intersectable;
class HitDetail;
class Ray;

class HitProbe final
{
public:
	inline HitProbe() :
		m_hitStack(),
		m_hitRayT(std::numeric_limits<real>::infinity()),
		m_cache(),
		m_hitDetailChannel(0)
	{}

	void calcIntersectionDetail(const Ray& ray, HitDetail* out_detail);
	bool isOnDefaultChannel() const;

	// Pushes a hit target that will aprticipate in hit detail's calculation 
	// onto the stack.
	inline void pushIntermediateHit(const Intersectable* const hitTarget)
	{
		m_hitStack.push(hitTarget);
	}

	// Similar to pushIntermediateHit(), except the parametric hit distance 
	// <hitRayT> must also be reported.
	inline void pushBaseHit(const Intersectable* const hitTarget,
	                        const real hitRayT)
	{
		m_hitStack.push(hitTarget);
		m_hitRayT = hitRayT;
	}

	inline void popIntermediateHit()
	{
		m_hitStack.pop();
	}

	inline void replaceCurrentHitWith(const Intersectable* const newCurrentHit)
	{
		m_hitStack.pop();
		m_hitStack.push(newCurrentHit);
	}

	inline void setChannel(const uint32 channel)
	{
		m_hitDetailChannel = channel;
	}

	inline const Intersectable* getCurrentHit() const
	{
		return m_hitStack.top();
	}

	inline real getHitRayT() const
	{
		return m_hitRayT;
	}

	inline uint32 getChannel() const
	{
		return m_hitDetailChannel;
	}

	// Clears the probe object and make it ready for probing again. 
	// 
	inline void clear()
	{
		m_hitStack.clear();
		m_hitRayT = std::numeric_limits<real>::infinity();
		m_hitDetailChannel = 0;
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

	inline Vector3R getCachedReal3(const int32 headIndex) const
	{
		Vector3R result;
		getCachedReal3(headIndex, &result);
		return result;
	}

	template<typename T>
	const T* getCache() const;

private:
	using Stack = TFixedSizeStack<const Intersectable*, PH_HIT_PROBE_DEPTH>;
	constexpr static std::size_t CACHE_SIZE = (PH_HIT_PROBE_CACHE_BYTES * 8 + (CHAR_BIT - 1)) / CHAR_BIT;

	Stack         m_hitStack;
	real          m_hitRayT;
	uint32        m_hitDetailChannel;

	// unsigned char is guaranteed to not have any padding, and 
	// does not violate strict aliasing rule
	unsigned char m_cache[CACHE_SIZE];
};

// In-header Implementations:

template<typename T>
inline const T* HitProbe::getCache() const
{
	static_assert(sizeof(T) <= sizeof(m_cache), 
		"not enough cache, consider increasing config.PH_HIT_PROBE_CACHE_BYTES");

	return reinterpret_cast<const T*>(&m_cache[0]);
}

}// end namespace ph