#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace ph
{

namespace detail::stats
{

struct TimeCounter final
{
	const std::string    name;
	const std::string    category;
	std::atomic_uint64_t totalMicroseconds;
	std::atomic_uint64_t count;

	TimeCounter(std::string name, std::string category);
	void addMicroseconds(std::uint64_t microseconds);
};

struct ScopedTimer final
{
	using Clock = std::chrono::steady_clock;

	TimeCounter&      counter;
	Clock::time_point startTime;

	explicit ScopedTimer(TimeCounter& counter);
	~ScopedTimer();
};

}// end namespace detail::stats

class TimerStatsReport final
{
public:
	struct TimeRecord final
	{
		std::string   name;
		std::string   category;
		std::uint64_t totalMicroseconds;
		std::uint64_t count;

		TimeRecord();
	};

	struct GroupedTimeRecord final
	{
		std::string   groupName;
		std::uint64_t totalMicroseconds;
		std::uint64_t count;

		std::vector<GroupedTimeRecord> subgroups;

		GroupedTimeRecord();
	};

	TimerStatsReport();

	GroupedTimeRecord getGroupedTimeRecord() const;
	std::string proportionalReport() const;
	std::string averagedReport() const;
	std::string detailedReport() const;
	std::string rawReport() const;

private:
	enum class EGroupedReport
	{
		ProportionOnly,
		AverageOnly,
		ProportionWithAverage,
		ProportionWithAverageAndTotal
	};

	std::vector<TimeRecord> m_records;

	static GroupedTimeRecord makeGroupedTimeRecordRecursive(const std::vector<TimeRecord>& records);

	static std::string makeGroupedReportRecursive(
		const GroupedTimeRecord& records, 
		EGroupedReport           reportType,
		const std::string&       linePrefix);
};

}// end namespace ph

#define PH_DEFINE_INLINE_TIMER_STAT(statName, categoryName)\
	inline ::ph::detail::stats::TimeCounter& internal_impl_time_counter_access_##statName()\
	{\
		static ::ph::detail::stats::TimeCounter counter(#statName, #categoryName);\
		return counter;\
	}

#define PH_DEFINE_INTERNAL_TIMER_STAT(statName, categoryName)\
	namespace\
	{\
		PH_DEFINE_INLINE_TIMER_STAT(statName, categoryName);\
	}

#define PH_DEFINE_EXTERNAL_TIMER_STAT(statName, categoryName)\
	PH_DEFINE_INLINE_TIMER_STAT(statName, categoryName)

#define PH_SCOPED_TIMER(statName)\
	::ph::detail::stats::ScopedTimer internal_impl_scopedTimer##statName(\
		internal_impl_time_counter_access_##statName())
