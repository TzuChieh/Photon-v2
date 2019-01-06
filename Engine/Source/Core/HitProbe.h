#pragma once

#include "Common/primitive_type.h"
#include "Common/config.h"
#include "Utility/TFixedSizeStack.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"

#include <limits>
#include <array>
#include <type_traits>
#include <cstddef>
#include <cstring>

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
		m_hitRayT(std::numeric_limits<real>::max()),
		m_hitDetailChannel(0),
		m_cache(),
		m_cacheHead(0)
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

	// Clears the probe object and makes it ready for probing again. 
	void clear();

	template<typename T>
	void cache(const T& data);

	template<typename T>
	void getCached(T* out_data);

private:
	using Stack = TFixedSizeStack<const Intersectable*, PH_HIT_PROBE_DEPTH>;

	Stack       m_hitStack;
	real        m_hitRayT;
	uint32      m_hitDetailChannel;
	std::byte   m_cache[PH_HIT_PROBE_CACHE_BYTES];
	std::size_t m_cacheHead;
};

// In-header Implementations:

inline void HitProbe::clear()
{
	m_hitStack.clear();
	m_hitRayT = std::numeric_limits<real>::max();
	m_hitDetailChannel = 0;
	m_cacheHead = 0;
}

template<typename T>
inline void HitProbe::cache(const T& data)
{
	static_assert(std::is_trivially_copyable_v<T>,
		"target type is not cacheable");
	static_assert(sizeof(T) <= sizeof(m_cache),
		"not enough cache to store target type, consider increasing config.PH_HIT_PROBE_CACHE_BYTES");

	PH_ASSERT_MSG(m_cacheHead + sizeof(T) <= sizeof(m_cache), 
		"ran out of cache, consider increasing config.PH_HIT_PROBE_CACHE_BYTES \n"
		"m_cacheHead     = " + std::to_string(m_cacheHead) + "\n" + 
		"sizeof(T)       = " + std::to_string(sizeof(T)) + "\n" + 
		"sizeof(m_cache) = " + std::to_string(sizeof(m_cache)));

	std::memcpy(m_cache + m_cacheHead, &data, sizeof(T));
	m_cacheHead += sizeof(T);
}

template<typename T>
inline void HitProbe::getCached(T* const out_data)
{
	static_assert(std::is_trivially_copyable_v<T> && sizeof(T) <= sizeof(m_cache));

	PH_ASSERT_IN_RANGE_INCLUSIVE(m_cacheHead, sizeof(T), sizeof(m_cache));
	m_cacheHead -= sizeof(T);
	std::memcpy(out_data, m_cache + m_cacheHead, sizeof(T));
}

}// end namespace ph