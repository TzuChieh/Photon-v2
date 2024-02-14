#pragma once

#include "Utility/TArrayStack.h"
#include "Math/TVector3.h"

#include <Common/assertion.h>
#include <Common/config.h>
#include <Common/primitive_type.h>

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
	/*!
	Creates an empty probe object and makes it ready for probing. To clear a used probe, assign an
	empty probe to it.
	*/
	HitProbe();

	void calcIntersectionDetail(const Ray& ray, HitDetail* out_detail);
	bool isOnDefaultChannel() const;

	/*!
	Pushes a hit target that will participate in hit detail's calculation onto the stack.
	*/
	void pushIntermediateHit(const Intersectable* hitTarget);

	/*!
	Similar to `pushIntermediateHit()`, except the parametric hit distance `hitRayT` must also 
	be reported.
	*/
	void pushBaseHit(const Intersectable* hitTarget, real hitRayT);

	void popIntermediateHit();

	void replaceCurrentHitWith(const Intersectable* newCurrentHit);

	void setChannel(uint8 channel);
	uint8 getChannel() const;

	const Intersectable* getCurrentHit() const;
	real getHitRayT() const;

	template<typename T>
	void pushCache(const T& data);

	template<typename T>
	T popCache();

private:
	using Stack = TArrayStack<const Intersectable*, PH_HIT_PROBE_DEPTH>;

	Stack       m_hitStack;
	real        m_hitRayT;
	std::byte   m_cache[PH_HIT_PROBE_CACHE_BYTES];
	uint8       m_cacheHead;
	uint8       m_hitDetailChannel;
};

// In-header Implementations:

inline HitProbe::HitProbe()
	: m_hitStack        ()
	, m_hitRayT         (std::numeric_limits<real>::max())
	, m_cache           ()
	, m_cacheHead       (0)
	, m_hitDetailChannel(0)
{}

inline void HitProbe::pushIntermediateHit(const Intersectable* const hitTarget)
{
	m_hitStack.push(hitTarget);
}

inline void HitProbe::pushBaseHit(const Intersectable* const hitTarget, const real hitRayT)
{
	m_hitStack.push(hitTarget);
	m_hitRayT = hitRayT;
}

inline void HitProbe::popIntermediateHit()
{
	m_hitStack.pop();
}

inline void HitProbe::replaceCurrentHitWith(const Intersectable* const newCurrentHit)
{
	m_hitStack.pop();
	m_hitStack.push(newCurrentHit);
}

inline void HitProbe::setChannel(const uint8 channel)
{
	m_hitDetailChannel = channel;
}

inline uint8 HitProbe::getChannel() const
{
	return m_hitDetailChannel;
}

inline const Intersectable* HitProbe::getCurrentHit() const
{
	return m_hitStack.top();
}

inline real HitProbe::getHitRayT() const
{
	return m_hitRayT;
}

template<typename T>
inline void HitProbe::pushCache(const T& data)
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
inline T HitProbe::popCache()
{
	static_assert(std::is_trivially_copyable_v<T>);
	static_assert(sizeof(T) <= sizeof(m_cache));

	PH_ASSERT_IN_RANGE_INCLUSIVE(m_cacheHead, sizeof(T), sizeof(m_cache));
	m_cacheHead -= sizeof(T);

	T data;
	std::memcpy(&data, m_cache + m_cacheHead, sizeof(T));
	return data;
}

}// end namespace ph
