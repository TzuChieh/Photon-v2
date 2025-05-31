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
	using ListType = TArrayVector<VolumeInteriorRecord, PH_VOLUME_TRACKER_MAX_SIZE>;

	auto findPreviousRecord(const SurfaceHit& X) -> ListType::IteratorType;

	ListType m_interiorList;
	uint16 m_maxPriority = 0;

#if PH_VOLUME_TRACKER_COLLECT_STATS
public:
	static inline std::atomic_uint64_t inconsistentRecordCount;
#endif
};

inline bool VolumeTracker::isTrueHit(const SurfaceHit& X) const
{
	const auto encounteredPriority = X.getMetadata().getInteriorPriority();

	return 
		// Always true hit with default priority
		(m_maxPriority == 0 && encounteredPriority == 0) ||

		// Equal priority indicates false hit
		(encounteredPriority > m_maxPriority);
}

inline void VolumeTracker::enterSurface(const SurfaceHit& X)
{
	const auto prevRecord = findPreviousRecord(X);
	if(prevRecord != m_interiorList.end())
	{
		prevRecord->count++;
	}
	else
	{
		const auto& metadata = X.getMetadata();
		m_interiorList.pushBack(VolumeInteriorRecord{
			.metadata = &metadata,
			.priority = metadata.getInteriorPriority(),
			.count = 1});

		m_maxPriority = std::max(metadata.getInteriorPriority(), m_maxPriority);
	}
}

inline void VolumeTracker::exitSurface(const SurfaceHit& X)
{
	const auto prevRecord = findPreviousRecord(X);
	if(prevRecord != m_interiorList.end())
	{
		prevRecord->count--;
		if(prevRecord->count == 0)
		{
			m_interiorList.removeBySwapPop(prevRecord - m_interiorList.begin());

			// Update max priority after removing a record
			m_maxPriority = 0;
			for(const VolumeInteriorRecord& record : m_interiorList)
			{
				m_maxPriority = std::max(static_cast<uint16>(record.priority), m_maxPriority);
			}
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

inline auto VolumeTracker::findPreviousRecord(const SurfaceHit& X) -> ListType::IteratorType
{
	return std::find_if(
		m_interiorList.begin(),
		m_interiorList.end(),
		[&X](const VolumeInteriorRecord& record)
		{
			return record.metadata == &X.getMetadata();
		});
}

}// end namespace ph::lta
