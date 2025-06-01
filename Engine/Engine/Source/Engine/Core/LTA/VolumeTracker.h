#pragma once

#include "Engine/Utility/TArrayVector.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"

#include <Common/config.h>
#include <Common/primitive_type.h>
#include <Common/assertion.h>

#include <cstddef>
#include <algorithm>
#include <limits>

#if PH_VOLUME_TRACKER_COLLECT_STATS
#include <atomic>
#endif

namespace ph::lta
{

struct VolumeInteriorRecord final
{
	const PrimitiveMetadata* metadata = nullptr;
	uint32 priority : 16 = 0;
	uint32 count : 8 = 0;
};

class VolumeTracker final
{
public:
	bool isTrueHit(const SurfaceHit& X) const;
	void enterSurface(const SurfaceHit& X);
	void exitSurface(const SurfaceHit& X);

private:
	using List = TArrayVector<VolumeInteriorRecord, PH_VOLUME_TRACKER_MAX_SIZE>;
	using PriorityIndex = uint8;

	auto findPriority(uint16 priority) -> List::IteratorType;
	auto findMaxPriority() const -> List::ConstIteratorType;

	List m_interiorList;
	PriorityIndex m_maxPriorityIdx = 0;

#if PH_VOLUME_TRACKER_COLLECT_STATS
public:
	static inline std::atomic_uint64_t inconsistentRecordCount;
#endif

	static_assert(std::numeric_limits<PriorityIndex>::max() >= PH_VOLUME_TRACKER_MAX_SIZE - 1,
		"Priority index type must be able to hold largest index.");
};

inline bool VolumeTracker::isTrueHit(const SurfaceHit& X) const
{
	const auto encounteredPriority = X.getMetadata().getInteriorPriority();

	return
		// Always true hit if nothing is encountered
		m_interiorList.isEmpty() ||

		// Always true hit with default priority
		(m_interiorList[m_maxPriorityIdx].priority == 0 && encounteredPriority == 0) ||

		// Equal priority indicates false hit
		(encounteredPriority > m_interiorList[m_maxPriorityIdx].priority);
}

inline void VolumeTracker::enterSurface(const SurfaceHit& X)
{
	const auto newPriority = X.getMetadata().getInteriorPriority();

	const auto prevRecord = findPriority(newPriority);
	if(prevRecord != m_interiorList.end())
	{
		prevRecord->count++;
	}
	else
	{
		const auto& metadata = X.getMetadata();
		m_interiorList.pushBack(VolumeInteriorRecord{
			.metadata = &metadata,
			.priority = newPriority,
			.count = 1});

		// Update max priority after adding a record
		if(m_interiorList.size() >= 2)
		{
			m_maxPriorityIdx = newPriority > m_interiorList[m_maxPriorityIdx].priority
				? static_cast<PriorityIndex>(m_interiorList.size() - 1)
				: m_maxPriorityIdx;
		}
		else
		{
			m_maxPriorityIdx = static_cast<PriorityIndex>(m_interiorList.size() - 1);
		}
	}
}

inline void VolumeTracker::exitSurface(const SurfaceHit& X)
{
	const auto newPriority = X.getMetadata().getInteriorPriority();

	const auto prevRecord = findPriority(newPriority);
	if(prevRecord != m_interiorList.end())
	{
		prevRecord->count--;
		if(prevRecord->count == 0)
		{
			m_interiorList.removeBySwapPop(prevRecord - m_interiorList.begin());

			// Update max priority after removing a record
			m_maxPriorityIdx = static_cast<PriorityIndex>(findMaxPriority() - m_interiorList.begin());
		}
	}
	// This can happen due to numerical error, or tracker not initialied with proper interior list.
	// E.g., missed due to ray offset to avoid self-intersection.
	else
	{
#if PH_VOLUME_TRACKER_COLLECT_STATS
		inconsistentRecordCount.fetch_add(1, std::memory_order_relaxed);
#endif
	}
}

inline auto VolumeTracker::findPriority(uint16 priority) -> List::IteratorType
{
	return std::find_if(
		m_interiorList.begin(),
		m_interiorList.end(),
		[priority](const VolumeInteriorRecord& record)
		{
			return record.priority == priority;
		});
}

inline auto VolumeTracker::findMaxPriority() const -> List::ConstIteratorType
{
	return std::max_element(
		m_interiorList.begin(),
		m_interiorList.end(),
		[](const VolumeInteriorRecord& a, const VolumeInteriorRecord& b)
		{
			return a.priority > b.priority;
		});
}

}// end namespace ph::lta
