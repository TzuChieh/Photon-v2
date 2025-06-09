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
#include <string>
#include <format>
#endif

namespace ph { class VolumeOptics; }

namespace ph::lta
{

struct VolumeInteriorRecord final
{
	const PrimitiveMetadata* metadata = nullptr;

	// Store here so we save an indirection
	uint32 priority : 16 = 0;
};

class VolumeTracker final
{
public:
	bool isTrueHit(const SurfaceHit& X) const;
	const PrimitiveMetadata* getCurrentVolumeMetadata(const PrimitiveMetadata* defaultMetadata = nullptr) const;
	const VolumeOptics* getCurrentVolumeOptics(const VolumeOptics* defaultOptics = nullptr) const;
	void enterSurface(const SurfaceHit& X);
	void exitSurface(const SurfaceHit& X);

private:
	using List = TArrayVector<VolumeInteriorRecord, PH_VOLUME_TRACKER_MAX_SIZE>;
	using PriorityIndex = uint8;

	auto findRecord(const SurfaceHit& X) -> List::IteratorType;
	auto findRecordWithMaxPriority() const -> List::ConstIteratorType;

	List m_interiorList;
	PriorityIndex m_maxPriorityIdx = 0;

#if PH_VOLUME_TRACKER_COLLECT_STATS
private:
	static inline std::atomic_uint64_t recordCount;
	static inline std::atomic_uint64_t inconsistentRecordCount;

public:
	static void initStats();
	static std::string reportStats();
#endif

	static_assert(std::numeric_limits<PriorityIndex>::max() >= PH_VOLUME_TRACKER_MAX_SIZE - 1,
		"Priority index type must be able to hold largest index.");
};

inline bool VolumeTracker::isTrueHit(const SurfaceHit& X) const
{
	// Always true hit if nothing is recorded; default/disabled priority are handled here
	if(m_interiorList.isEmpty())
	{
		return true;
	}

	const PrimitiveMetadata& encounteredMetadata = X.getMetadata();

	return
		// Could be the exiting hit
		&encounteredMetadata == m_interiorList[m_maxPriorityIdx].metadata ||

		// Equal and lower priorities indicate false hit; when equal, keep current volume properties
		(encounteredMetadata.getInteriorPriority() > m_interiorList[m_maxPriorityIdx].priority);
}

inline const PrimitiveMetadata* VolumeTracker::getCurrentVolumeMetadata(const PrimitiveMetadata* defaultMetadata) const
{
	PH_ASSERT(
		m_interiorList.isEmpty() || 
		(m_maxPriorityIdx < m_interiorList.size() && m_interiorList[m_maxPriorityIdx].priority > 0));

	return !m_interiorList.isEmpty()
		? m_interiorList[m_maxPriorityIdx].metadata
		: defaultMetadata;
}

inline const VolumeOptics* VolumeTracker::getCurrentVolumeOptics(const VolumeOptics* defaultOptics) const
{
	const PrimitiveMetadata* metadata = getCurrentVolumeMetadata();
	return metadata
		? metadata->getInterior().getOptics()
		: defaultOptics;
}

inline void VolumeTracker::enterSurface(const SurfaceHit& X)
{
	// Default/disabled priority is not explicitly tracked
	const auto newPriority = X.getMetadata().getInteriorPriority();
	if(newPriority == 0)
	{
		return;
	}

#if PH_VOLUME_TRACKER_COLLECT_STATS
	recordCount.fetch_add(1, std::memory_order_relaxed);
#endif

	// This can happen due to numerical error, tracker not initialized with proper interior list,
	// primitive ID collision, bad geometry normal/face, missed due to ray offset to avoid
	// self-intersection, etc.
	const auto prevRecord = findRecord(X);
	if(prevRecord != m_interiorList.end())
	{
#if PH_VOLUME_TRACKER_COLLECT_STATS
		inconsistentRecordCount.fetch_add(1, std::memory_order_relaxed);
#endif
		exitSurface(X);
		return;
	}

	const auto& metadata = X.getMetadata();
	m_interiorList.pushBack(VolumeInteriorRecord{
		.metadata = &metadata,
		.priority = newPriority});

	// Update max priority after adding a record
	if(m_interiorList.size() >= 2)
	{
		m_maxPriorityIdx = newPriority > m_interiorList[m_maxPriorityIdx].priority
			? static_cast<PriorityIndex>(m_interiorList.size() - 1)
			: m_maxPriorityIdx;
	}
	else
	{
		m_maxPriorityIdx = 0;
	}
}

inline void VolumeTracker::exitSurface(const SurfaceHit& X)
{
	// Default/disabled priority is not explicitly tracked
	const auto oldPriority = X.getMetadata().getInteriorPriority();
	if(oldPriority == 0)
	{
		return;
	}

#if PH_VOLUME_TRACKER_COLLECT_STATS
	recordCount.fetch_add(1, std::memory_order_relaxed);
#endif

	const auto prevRecord = findRecord(X);
	if(prevRecord != m_interiorList.end())
	{
		m_interiorList.removeBySwapPop(prevRecord - m_interiorList.begin());

		// Update max priority after removing a record
		m_maxPriorityIdx = static_cast<PriorityIndex>(findRecordWithMaxPriority() - m_interiorList.begin());
	}
	// This can happen due to numerical error, tracker not initialied with proper interior list,
	// primitive ID collision, etc. E.g., missed due to ray offset to avoid self-intersection.
	else
	{
#if PH_VOLUME_TRACKER_COLLECT_STATS
		inconsistentRecordCount.fetch_add(1, std::memory_order_relaxed);
#endif
		return;
	}
}

inline auto VolumeTracker::findRecord(const SurfaceHit& X) -> List::IteratorType
{
	const PrimitiveMetadata* metadata = &X.getMetadata();
	return std::find_if(
		m_interiorList.begin(),
		m_interiorList.end(),
		[metadata](const VolumeInteriorRecord& record)
		{
			return record.metadata == metadata;
		});
}

inline auto VolumeTracker::findRecordWithMaxPriority() const -> List::ConstIteratorType
{
	return std::max_element(
		m_interiorList.begin(),
		m_interiorList.end(),
		[](const VolumeInteriorRecord& a, const VolumeInteriorRecord& b)
		{
			return a.priority < b.priority;
		});
}

#if PH_VOLUME_TRACKER_COLLECT_STATS

inline void VolumeTracker::initStats()
{
	recordCount = 0;
	inconsistentRecordCount = 0;
}

inline std::string VolumeTracker::reportStats()
{
	return std::format(
		"volume tracker inconsistent record count: {}/{} ({}%)",
		inconsistentRecordCount.load(),
		recordCount.load(),
		100.0 * inconsistentRecordCount.load() / recordCount.load());
}

#endif

}// end namespace ph::lta
