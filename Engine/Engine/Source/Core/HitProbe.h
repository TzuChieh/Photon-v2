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

/*! @brief Lightweight ray intersection testing and reporting object.
If an intersection is found, a detailed report can be obtained from the stored information.
*/
class HitProbe final
{
public:
	/*!
	Creates an empty probe object and makes it ready for probing. To clear a used probe, assign an
	empty probe to it.
	*/
	HitProbe();

	/*! @brief Calculates basic hit information using this probe.
	The information is calculated using a copy of the current state of this probe.
	*/
	void calcHitDetail(const Ray& ray, HitDetail* out_detail) const;

	/*! @brief Calculates full hit information using this probe.
	The information is calculated using a copy of the current state of this probe.
	*/
	void calcFullHitDetail(const Ray& ray, HitDetail* out_detail) const;

	/*! @brief Intersect the intersected object again with a different ray.
	The operation is done using a copy of the current state of this probe.
	@param ray The different ray to use for intersection test.
	@param probe The probe to record the intersection.
	@param srcRay The ray from a previous hit event (associated with `this` probe).
	@note Generates hit event (with `ray` and `probe`).
	*/
	bool reintersect(const Ray& ray, HitProbe& probe, const Ray& srcRay) const;

	bool isOnDefaultChannel() const;

	/*! @brief Adds a hit target that will participate in hit detail's calculation to the stack.
	*/
	void pushIntermediateHit(const Intersectable* hitTarget);

	/*! @brief Adds the first hit target to the stack.
	Similar to `pushIntermediateHit()`, except the parametric hit distance `hitRayT` must also 
	be reported.
	*/
	void pushBaseHit(const Intersectable* hitTarget, real hitRayT);

	/*! @brief Removes the most recent hit target from the stack.
	*/
	void popHit();

	void replaceTopHit(const Intersectable* newTopHit);
	void replaceBaseHitRayT(real newHitRayT);

	void setChannel(uint8 channel);
	uint8 getChannel() const;

	/*!
	@return Most recent hit target on the stack. May be `nullptr` if the probe is empty.
	*/
	const Intersectable* getTopHit() const;

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
#if PH_DEBUG
	bool        m_hasBaseHitSet{false};
#endif
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

#if PH_DEBUG
	m_hasBaseHitSet = true;
#endif
}

inline void HitProbe::popHit()
{
	m_hitStack.pop();
}

inline void HitProbe::replaceTopHit(const Intersectable* const newTopHit)
{
	m_hitStack.pop();
	m_hitStack.push(newTopHit);
}

inline void HitProbe::replaceBaseHitRayT(const real newHitRayT)
{
	PH_ASSERT(m_hasBaseHitSet);
	m_hitRayT = newHitRayT;
}

inline void HitProbe::setChannel(const uint8 channel)
{
	m_hitDetailChannel = channel;
}

inline uint8 HitProbe::getChannel() const
{
	return m_hitDetailChannel;
}

inline const Intersectable* HitProbe::getTopHit() const
{
	return m_hitStack.top();
}

inline real HitProbe::getHitRayT() const
{
	PH_ASSERT(m_hasBaseHitSet);
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
