#include "Common/stats.h"

#include <Common/assertion.h>

#include <utility>
#include <mutex>
#include <format>
#include <unordered_map>

namespace ph
{

namespace detail::stats
{

namespace
{

inline std::mutex& TIME_STATS_MUTEX()
{
	static std::mutex locker;
	return locker;
}

inline std::vector<TimeCounter*>& TIME_COUNTERS()
{
	static std::vector<TimeCounter*> counters;
	return counters;
}

}// end anonymous namespace

TimeCounter::TimeCounter(std::string name, std::string category) :
	name(std::move(name)), category(std::move(category)), totalMicroseconds(0), count(0)
{
	std::lock_guard<std::mutex> lock(TIME_STATS_MUTEX());

	TIME_COUNTERS().push_back(this);
}

void TimeCounter::addMicroseconds(const std::uint64_t microseconds)
{
	count.fetch_add(1, std::memory_order_relaxed);
	totalMicroseconds.fetch_add(microseconds, std::memory_order_relaxed);
}

ScopedTimer::ScopedTimer(TimeCounter& counter) :
	counter(counter), startTime(Clock::now())
{}

ScopedTimer::~ScopedTimer()
{
	using TimeUnit = std::chrono::microseconds;

	const auto& duration = Clock::now() - startTime;
	const auto& delta    = std::chrono::duration_cast<TimeUnit>(duration).count();
	counter.addMicroseconds(static_cast<std::uint64_t>(delta));
}

}// end namespace detail::stats

TimerStatsReport::TimeRecord::TimeRecord() :
	name(), category(), totalMicroseconds(0), count(0)
{}

TimerStatsReport::GroupedTimeRecord::GroupedTimeRecord() :
	groupName(), totalMicroseconds(0), count(0), subgroups()
{}

TimerStatsReport::TimerStatsReport() :
	m_records()
{
	std::lock_guard<std::mutex> lock(detail::stats::TIME_STATS_MUTEX());

	m_records.reserve(detail::stats::TIME_COUNTERS().size());
	for(const auto& timeCounter : detail::stats::TIME_COUNTERS())
	{
		TimeRecord record;
		record.name              = timeCounter->name;
		record.category          = timeCounter->category;
		record.totalMicroseconds = timeCounter->totalMicroseconds.load(std::memory_order_relaxed);
		record.count             = timeCounter->count.load(std::memory_order_relaxed);

		m_records.push_back(record);
	}
}

TimerStatsReport::GroupedTimeRecord TimerStatsReport::getGroupedTimeRecord() const
{
	return makeGroupedTimeRecordRecursive(m_records);
}

std::string TimerStatsReport::proportionalReport() const
{
	GroupedTimeRecord rootGroup = getGroupedTimeRecord();
	if(rootGroup.groupName.empty())
	{
		rootGroup.groupName = "Proportional Timing Report";
	}

	return makeGroupedReportRecursive(rootGroup, EGroupedReport::ProportionOnly, "+");
}

std::string TimerStatsReport::averagedReport() const
{
	GroupedTimeRecord rootGroup = getGroupedTimeRecord();
	if(rootGroup.groupName.empty())
	{
		rootGroup.groupName = "Averaged Timing Report";
	}

	return makeGroupedReportRecursive(rootGroup, EGroupedReport::AverageOnly, "+");
}

std::string TimerStatsReport::detailedReport() const
{
	GroupedTimeRecord rootGroup = getGroupedTimeRecord();
	if(rootGroup.groupName.empty())
	{
		rootGroup.groupName = "Detailed Timing Report [% time, avg. time] -> name (total time):";
	}

	return makeGroupedReportRecursive(rootGroup, EGroupedReport::ProportionWithAverageAndTotal, "+");
}

std::string TimerStatsReport::rawReport() const
{
	std::string reportStr;
	reportStr.reserve(128 * m_records.size());
	for(const TimeRecord& record : m_records)
	{
		reportStr += std::format(
			"category: {}, name: {}, total microseconds: {}, counts: {}\n",
			record.category, record.name, record.totalMicroseconds, record.count);
	}

	return reportStr;
}

TimerStatsReport::GroupedTimeRecord TimerStatsReport::makeGroupedTimeRecordRecursive(
	const std::vector<TimeRecord>& records)
{
	GroupedTimeRecord groupedRecord;

	std::unordered_map<std::string, std::vector<TimeRecord>> subgroupNameToRecords;
	for(const TimeRecord& record : records)
	{
		groupedRecord.totalMicroseconds += record.totalMicroseconds;
		groupedRecord.count             += record.count;

		if(record.category.empty())
		{
			GroupedTimeRecord leafGroup;
			leafGroup.groupName         = record.name;
			leafGroup.totalMicroseconds = record.totalMicroseconds;
			leafGroup.count             = record.count;

			groupedRecord.subgroups.push_back(leafGroup);
		}
		else
		{
			const auto underscorePos = record.category.find('_');
			const auto subgroupName = underscorePos != std::string::npos
				? record.category.substr(0, underscorePos)
				: record.category;

			TimeRecord subgroupRecord = record;
			subgroupRecord.category = underscorePos != std::string::npos
				? record.category.substr(underscorePos + 1)
				: "";

			subgroupNameToRecords[subgroupName].push_back(subgroupRecord);
		}
	}

	for(const auto& [subgroupName, subgroupRecords] : subgroupNameToRecords)
	{
		GroupedTimeRecord subgroup = makeGroupedTimeRecordRecursive(subgroupRecords);
		subgroup.groupName = subgroupName;

		groupedRecord.subgroups.push_back(subgroup);
	}

	return groupedRecord;
}

std::string TimerStatsReport::makeGroupedReportRecursive(
	const GroupedTimeRecord& records, 
	const EGroupedReport     reportType,
	const std::string&       linePrefix)
{
	std::string reportStr;
	reportStr.reserve(128 * records.subgroups.size());

	if(reportType != EGroupedReport::ProportionWithAverageAndTotal)
	{
		reportStr += std::format(
			"{}\n",
			records.groupName);
	}
	else
	{
		reportStr += std::format(
			"{} ({}ms)\n",
			records.groupName, records.totalMicroseconds / 1000);
	}

	for(const GroupedTimeRecord& subgroup : records.subgroups)
	{
		const auto subgroupPercentage =
			static_cast<double>(subgroup.totalMicroseconds) /
			static_cast<double>(records.totalMicroseconds) *
			100.0;

		const auto subgroupAverageMS = subgroup.count == 0 ? 0.0 :
			static_cast<double>(subgroup.totalMicroseconds / subgroup.count) / 1000.0;

		const std::string subgroupReport = makeGroupedReportRecursive(subgroup, reportType, linePrefix + "--");
		
		switch(reportType)
		{
		case EGroupedReport::ProportionOnly:
			reportStr += std::format(
				"{} [{:8.4f}%] -> {}",
				linePrefix, subgroupPercentage, subgroupReport);
			break;

		case EGroupedReport::AverageOnly:
			reportStr += std::format(
				"{} [{:.2f}ms] -> {}",
				linePrefix, subgroupAverageMS, subgroupReport);
			break;

		case EGroupedReport::ProportionWithAverage:
		case EGroupedReport::ProportionWithAverageAndTotal:
			reportStr += std::format(
				"{} [{:6.2f}% | {:.2f}ms] -> {}",
				linePrefix, subgroupPercentage, subgroupAverageMS, subgroupReport);
			break;

		default:
			PH_ASSERT_UNREACHABLE_SECTION();
			reportStr += "(report error)";
			break;
		}
	}

	return reportStr;
}

}// end namespace ph
