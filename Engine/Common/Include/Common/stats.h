#pragma once

#include "Common/config.h"
#include "Common/primitive_type.h"

#include <chrono>
#include <cstddef>
#include <limits>
#include <map>
#include <string>
#include <vector>

namespace ph
{

namespace detail::stats
{

struct TimeCounter final
{
	using Clock = std::chrono::steady_clock;

	TimeCounter(std::string name, std::string path);

	TimeCounter(const TimeCounter& other) = delete;
	TimeCounter& operator = (const TimeCounter& rhs) = delete;
	TimeCounter(TimeCounter&& other) = delete;
	TimeCounter& operator = (TimeCounter&& rhs) = delete;

	void addSpan(
		Clock::time_point startTime,
		Clock::time_point endTime) const;

	const std::string    name;
	const std::string    path;
	uint32               id;
};

struct TimingRecord final
{
	using ClockRep = TimeCounter::Clock::duration::rep;

	void addSpan(ClockRep elapsedTicks);
	void merge(const TimingRecord& other);
	void clear();

	const TimeCounter* counter = nullptr;
	ClockRep           totalTicks = 0;
	uint64             count = 0;
	ClockRep           minTicks = std::numeric_limits<ClockRep>::max();
	ClockRep           maxTicks = 0;
};

struct ScopedTimer final
{
	using Clock = std::chrono::steady_clock;

	explicit ScopedTimer(const TimeCounter& counter);
	~ScopedTimer();

	ScopedTimer(const ScopedTimer& other) = delete;
	ScopedTimer& operator = (const ScopedTimer& rhs) = delete;

	const TimeCounter& counter;
	Clock::time_point  startTime;
};

inline ScopedTimer::ScopedTimer(const TimeCounter& counter)
	: counter(counter)
	, startTime(Clock::now())
{}

inline ScopedTimer::~ScopedTimer()
{
	const auto endTime = Clock::now();
	counter.addSpan(startTime, endTime);
}

}// end namespace detail::stats

class TimerStatsReport final
{
public:
	struct TimingNode final
	{
		std::string   groupName;
		uint64        totalMicroseconds = 0;
		uint64        minMicroseconds = std::numeric_limits<uint64>::max();
		uint64        maxMicroseconds = 0;
		uint64        count = 0;

		std::vector<TimingNode> subgroups;
	};

	TimerStatsReport();

	static void flushTimerStats();

	TimingNode getRootTimingNode() const;
	std::string detailedReport() const;

private:
	using TimingClockRep = detail::stats::TimeCounter::Clock::duration::rep;

	static uint64 ticksToMicroseconds(TimingClockRep ticks);
	static std::string formatDuration(uint64 microseconds, uint64 count = 0);
	static std::string formatCallCount(uint64 count);
	static std::string formatShare(uint64 totalMicroseconds, uint64 parentTotalMicroseconds);
	static std::string fitNameColumn(const std::string& name);
	static std::string makeFullTimingPath(const detail::stats::TimeCounter& counter);
	static std::string makeParentTimingPath(const std::string& path);
	static std::string makeTimingPathName(const std::string& path);
	static void addStatsFromNode(TimingNode& receivingNode, const TimingNode& sourceNode);
	static TimingNode makeTimingNode(
		const std::map<std::string, TimingNode>& pathToNodes,
		const std::string& path);
	static TimingNode makeRootTimingNode(const std::vector<detail::stats::TimingRecord>& records);
	static std::string makeTimingNodeReportRecursive(
		const TimingNode& node,
		const std::string& linePrefix,
		uint64 parentTotalMicroseconds);

	std::vector<detail::stats::TimingRecord> m_records;
};

}// end namespace ph

#if PH_STATS

#define PH_DEFINE_INLINE_TIMER_STAT(statName, statPath)\
	inline ::ph::detail::stats::TimeCounter& internal_impl_time_counter_access_##statName()\
	{\
		static ::ph::detail::stats::TimeCounter counter(#statName, #statPath);\
		return counter;\
	}

#define PH_DEFINE_INTERNAL_TIMER_STAT(statName, statPath)\
	namespace\
	{\
		PH_DEFINE_INLINE_TIMER_STAT(statName, statPath);\
	}

#define PH_DEFINE_EXTERNAL_TIMER_STAT(statName, statPath)\
	PH_DEFINE_INLINE_TIMER_STAT(statName, statPath)

#define PH_SCOPED_TIMER(statName)\
	::ph::detail::stats::ScopedTimer internal_impl_scopedTimer##statName(\
		internal_impl_time_counter_access_##statName())

#define PH_FLUSH_TIMER_STATS()\
	::ph::TimerStatsReport::flushTimerStats()

#else

#define PH_DEFINE_INLINE_TIMER_STAT(statName, statPath)
#define PH_DEFINE_INTERNAL_TIMER_STAT(statName, statPath)
#define PH_DEFINE_EXTERNAL_TIMER_STAT(statName, statPath)
#define PH_SCOPED_TIMER(statName)
#define PH_FLUSH_TIMER_STATS()

#endif
